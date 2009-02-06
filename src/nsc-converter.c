/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 *  nsc-converter.c
 * 
 *  Copyright (C) 2008 Brian Pepple
 *  Copyright (C) 2003 Ross Burton
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Author: Brian Pepple <bpepple@fedoraproject.org>
 *          Ross Burton <ross@burtonini.com>
 * 
 */

#include <config.h>

#include <sys/time.h>
#include <string.h>

#include <gconf/gconf-client.h>
#include <glib/gi18n.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkfilechooser.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmessagedialog.h>
#include <gtk/gtkprogressbar.h>
#include <gtk/gtkwidget.h>
#include <gst/gst.h>
#include <libnautilus-extension/nautilus-file-info.h>
#include <profiles/gnome-media-profiles.h>

#include "nsc-converter.h"
#include "nsc-gstreamer.h"
#include "nsc-xml.h"

typedef struct _NscConverterPrivate NscConverterPrivate;

typedef struct {
	int            seconds;
	struct timeval time;
	int            ripped;
	int            taken;
} Progress;

struct _NscConverterPrivate {
	/* GStreamer Object */
	NscGStreamer	*gst;

	/* The current audio profile */
	GMAudioProfile *profile;

	GtkWidget	*dialog;
	GtkWidget	*path_chooser;
	GtkWidget       *profile_chooser;
	GtkWidget       *progress_dlg;
	GtkWidget       *progressbar;
	GtkWidget       *speedbar;
	
	/* Files to be convertered */
	GList		*files;
	gint             files_converted;
	gint		 total_files;

	/* Directory to save new file */
	gchar           *save_path;

	/* Snapshots of the progress used to calculate the speed and the ETA */
	Progress         before;

        /* The duration of the file being converter. */
	gint             current_duration;

	/* The total duration of the file being converter. */
	gint             total_duration;
};

/* Default profile name */
#define DEFAULT_AUDIO_PROFILE_NAME "cdlossy"

#define NSC_CONVERTER_GET_PRIVATE(o)           \
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), NSC_TYPE_CONVERTER, NscConverterPrivate))

G_DEFINE_TYPE (NscConverter, nsc_converter, G_TYPE_OBJECT)

enum {
	PROP_FILES = 1,
};

static void
nsc_converter_finalize (GObject *object)
{
	NscConverter 	    *conv = NSC_CONVERTER (object);
	NscConverterPrivate *priv = NSC_CONVERTER_GET_PRIVATE (conv);

	if (priv->save_path)
		g_free (priv->save_path);

	if (priv->gst)
		g_object_unref (priv->gst);

	if (priv->profile)
		g_object_unref (priv->profile);

	if (priv->files)
		g_list_free (priv->files);

	G_OBJECT_CLASS (nsc_converter_parent_class)->finalize(object);
}

static void
nsc_converter_set_property (GObject      *object,
			    guint         property_id,
			    const GValue *value,
			    GParamSpec   *pspec)
{
	NscConverter        *self = NSC_CONVERTER (object);
	NscConverterPrivate *priv = NSC_CONVERTER_GET_PRIVATE (self);

	switch (property_id) {
	case PROP_FILES:
		priv->files = g_value_get_pointer (value);
		priv->total_files = g_list_length (priv->files);
		break;
	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
nsc_converter_get_property (GObject    *object,
			    guint       property_id,
			    GValue     *value,
			    GParamSpec *pspec)
{
	NscConverter	    *self = NSC_CONVERTER (object);
	NscConverterPrivate *priv = NSC_CONVERTER_GET_PRIVATE (self);

	switch (property_id) {
	case PROP_FILES:
		g_value_set_pointer (value, priv->files);
		break;
	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
nsc_converter_class_init (NscConverterClass *klass)
{
	g_type_class_add_private (klass, sizeof (NscConverterPrivate));

	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GParamSpec   *files_param_spec;

	object_class->finalize = nsc_converter_finalize;
	object_class->set_property = nsc_converter_set_property;
	object_class->get_property = nsc_converter_get_property;

	files_param_spec =
		g_param_spec_pointer ("files",
				      "Files",
				      "Set selected files",
				      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_property (object_class,
					 PROP_FILES,
					 files_param_spec);
}

/**
 * Cancel converting the files.
 */
static void
progress_cancel_cb (GtkWidget *widget, gpointer user_data)
{
	NscConverter        *conv;
	NscConverterPrivate *priv;

	conv =  NSC_CONVERTER (user_data);
	priv =  NSC_CONVERTER_GET_PRIVATE (conv);

	nsc_gstreamer_cancel_convert (priv->gst);
	g_object_unref (priv->gst);
	priv->gst = NULL;

	/* TODO: Remove the file that was partially converted */

	gtk_widget_destroy (priv->progress_dlg);
}

/**
 * Create the progress dialog
 */
static void
create_progress_dialog (NscConverter *converter)
{
	NscConverterPrivate *priv;
	GtkWidget           *button;

	priv = NSC_CONVERTER_GET_PRIVATE (converter);

	/* Create the gtkbuilder, and grab the widgets */
	nsc_xml_get_file ("progress.xml",
			  "progress_dialog", &priv->progress_dlg,
			  "file_progressbar", &priv->progressbar,
			  "speed_progressbar", &priv->speedbar,
			  "cancel_button", &button,
			  NULL);

	/* Connect the signal for the cancel button */
	g_signal_connect (G_OBJECT (button), "clicked",
			  (GCallback) progress_cancel_cb,
			  converter);
}

/**
 * Create the new GFile.  This will need to be unreferenced.
 */
static GFile *
create_new_file (NscConverter *converter, GFile *file)
{
	NscConverterPrivate *priv;
	GFile               *new_file;
	gchar               *old_basename, *new_basename;
	gchar               *extension, *new_uri;
	const gchar         *new_extension;

	g_return_val_if_fail (G_IS_FILE (file), NULL);

	priv = NSC_CONVERTER_GET_PRIVATE (converter);

	/* Let's the get the basename from the original file */
	old_basename = g_file_get_basename (file);

	/* Now let's remove the extension from the basename. */
	extension = g_strdup (strrchr (old_basename, '.'));
	if (extension != NULL)
		old_basename[strlen (old_basename) - strlen (extension)] = '\0';
	g_free (extension);

	/* Get the new extension from the audio profie */
	new_extension = gm_audio_profile_get_extension (priv->profile);

	/* Create the new basename */
	new_basename = g_strdup_printf ("%s.%s", old_basename, new_extension);
	g_free (old_basename);

	/* Now let's create the new files uri */
	new_uri = g_strconcat (priv->save_path, G_DIR_SEPARATOR_S,
			       new_basename, NULL);
	g_free (new_basename);

	/* And now finally let's create the new GFile */
	new_file = g_file_new_for_uri (new_uri);
	g_free (new_uri);

	return new_file;
}

/**
 * Function to get orginal & new files, and pass
 * them to the gstreamer object.
 */
static void
convert_file (NscConverter *convert)
{
	NscConverterPrivate *priv;
	NautilusFileInfo    *file_info;
	GFile               *old_file, *new_file;
	GError              *err = NULL;

	priv = NSC_CONVERTER_GET_PRIVATE (convert);

	g_return_if_fail (priv->files != NULL);

	/* Get the files */
	file_info = NAUTILUS_FILE_INFO (priv->files->data);
	old_file = nautilus_file_info_get_location (file_info);
	new_file = create_new_file (convert, old_file);

	/* Let's finally get to the fun stuff */
	nsc_gstreamer_convert_file (priv->gst, old_file, new_file,
				    &err);

	/* Free the files since we do not need them anymore */
	g_object_unref (old_file);
	g_object_unref (new_file);
}

/**
 * Update progressbar text
 */
static void
update_progressbar_text (NscConverter *convert)
{
	NscConverterPrivate *priv;
	gchar               *text;

	g_return_if_fail (NSC_IS_CONVERTER (convert));

	priv = NSC_CONVERTER_GET_PRIVATE (convert);

	text = g_strdup_printf (_("Converting: %d of %d"),
				priv->files_converted + 1, priv->total_files);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->progressbar),
				   text);
	g_free (text);
}

/** 
 * Callback to report errors.  The error passed in does not
 * need to be freed.
 */
static void
on_error_cb (NscGStreamer *gstream, GError *error, gpointer data)
{
	NscConverter	    *converter;
	NscConverterPrivate *priv;
	GtkWidget           *dialog;
	gchar               *text;

	converter = NSC_CONVERTER (data);
	priv = NSC_CONVERTER_GET_PRIVATE (converter);

	text = g_strdup_printf (_("Nautilus Sound Converter could "
				  "not convert this file.\nReason: %s"),
				error->message);

	dialog = gtk_message_dialog_new (GTK_WINDOW (priv->dialog), 0,
					 GTK_MESSAGE_ERROR,
					 GTK_BUTTONS_CLOSE,
					 text);
	g_free (text);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

/**
 * Callback to report completion.
 */
static void
on_completion_cb (NscGStreamer *gstream, gpointer data)
{
	NscConverter	    *converter;
	NscConverterPrivate *priv;
	gdouble              fraction;

	converter = NSC_CONVERTER (data);
	priv = NSC_CONVERTER_GET_PRIVATE (converter);
	
	/* Increment converted total & point to next file */
	priv->files_converted++;
	priv->files = priv->files->next;

	/* Reset the progress variables */
	priv->current_duration =  0;
	priv->total_duration   =  0;
	priv->before.seconds   = -1;

	/* Clear the speed label */
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->speedbar),
				   (_("Speed: Unknown")));

	/* Update the progress dialog */
	fraction = (double) priv->files_converted / priv->total_files;
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->progressbar),
				       fraction);
	update_progressbar_text (converter);

	/* If there are more files let's go ahead and convert them */
	if (priv->files != NULL) {
		convert_file (converter);
	} else {
		/* No more files to convert time to do some cleanup */
		g_object_unref (priv->gst);
		priv->gst = NULL;
		gtk_widget_destroy (priv->progress_dlg);
	}
}

/**
 * Callback to set file total duration.
 */
static void
on_duration_cb (NscGStreamer *gstream,
		const int     seconds,
		gpointer      data)
{
	NscConverter        *conv;
	NscConverterPrivate *priv;

	conv = NSC_CONVERTER (data);
	priv = NSC_CONVERTER_GET_PRIVATE (conv);

	priv->total_duration = seconds;
}

/**
 * Update the ETA and Speed labels
 */
static void
update_speed_progress (NscConverter *conv,
		       float         speed,
		       int           eta)
{
	NscConverterPrivate *priv;
	gchar               *eta_str;

	priv = NSC_CONVERTER_GET_PRIVATE (conv);

	if (eta >= 0) {
		eta_str =
			g_strdup_printf (_("Estimated time left: %d:%02d (at %0.1f\303\227)"),
					 eta / 60,
					 eta % 60,
					 speed);
	} else {
		eta_str = g_strdup (_("Estimated time left: unknown"));
	}

	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->speedbar),
				   eta_str);
	g_free (eta_str);
}

/**
 * Callback to report on file conversion progress.
 */
static void
on_progress_cb (NscGStreamer *gstream,
		const int     seconds,
		gpointer      data)
{
	NscConverter        *conv;
	NscConverterPrivate *priv;

	conv = NSC_CONVERTER (data);
	priv = NSC_CONVERTER_GET_PRIVATE (conv);

	if (priv->total_duration != 0) {
		float percent;

		percent =
			CLAMP ((float) (priv->current_duration + seconds)
			       / (float) priv->total_duration, 0, 1);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->speedbar), percent);

		if (priv->before.seconds == -1) {
			priv->before.seconds = priv->current_duration + seconds;
			gettimeofday (&priv->before.time, NULL);
		} else {
			struct timeval time;
			gint           taken;
			float          speed;

			gettimeofday (&time, NULL);
			taken = time.tv_sec + (time.tv_usec / 1000000.0)
				- (priv->before.time.tv_sec + (priv->before.time.tv_usec / 1000000.0));

			if (taken >= 2) {
				priv->before.taken += taken;
				priv->before.ripped += priv->current_duration + seconds - priv->before.seconds;
				speed = (float) priv->before.ripped / (float) priv->before.taken;
				update_speed_progress (conv, speed,
						       (int) ((priv->total_duration - priv->current_duration - seconds)
							      / speed));
				priv->before.seconds = priv->current_duration + seconds;
				gettimeofday (&priv->before.time, NULL);
			}
		}
	}
}

/**
 * The OK or Cancel button was pressed on the main dialog.
 */
static void
converter_response_cb (GtkWidget *dialog,
		       gint       response_id,
		       gpointer   user_data)
{
	if (response_id == GTK_RESPONSE_OK) {
		NscConverter	    *converter;
		NscConverterPrivate *priv;

		converter = NSC_CONVERTER (user_data);
		priv = NSC_CONVERTER_GET_PRIVATE (converter);

		/* Grab the save path */
		priv->save_path =
			g_strdup (gtk_file_chooser_get_uri
				  (GTK_FILE_CHOOSER (priv->path_chooser)));
		
		priv->profile =
			gm_audio_profile_choose_get_active (priv->profile_chooser);
	      
		/* This probably isn't necessary, but let's leave it for now */
		if (!(nsc_gstreamer_supports_profile (priv->profile))) {
			/*
			 * TODO: Add a message dialog to tell the user
			 *       the selected profile is not supported.
			 */
			return;
		}

		/* Create the gstreamer converter object */
		priv->gst = nsc_gstreamer_new (priv->profile);
	
		/* Connect to the gstreamer object signals */
		g_signal_connect (G_OBJECT (priv->gst), "completion",
				  (GCallback) on_completion_cb,
				  converter);
		g_signal_connect (G_OBJECT (priv->gst), "error",
				  (GCallback) on_error_cb,
				  converter);
		g_signal_connect (G_OBJECT (priv->gst), "progress",
				  (GCallback) on_progress_cb,
				  converter);
		g_signal_connect (G_OBJECT (priv->gst), "duration",
				  (GCallback) on_duration_cb,
				  converter);

		/* Create the progress window */
		create_progress_dialog (converter);

		/* Let's put some text in the progressbar */
		update_progressbar_text (converter);
		gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->speedbar),
					   (_("Speed: Unknown")));

		/* Alright we're finally ready to start converting */
		convert_file (converter);
	}
	gtk_widget_destroy (dialog);
}

/**
 * The Edit Profiles button was pressed.
 */
static void
converter_edit_profile (GtkButton *button,
			gpointer   user_data)
{
	NscConverterPrivate *priv;
	GConfClient         *gconf;
	GtkWidget           *dialog;

	priv = NSC_CONVERTER_GET_PRIVATE (user_data);

	gconf = gconf_client_get_default ();

	dialog = gm_audio_profiles_edit_new (gconf,
					     GTK_WINDOW (priv->dialog));

	g_object_unref (gconf);

	gtk_widget_show_all (dialog);
	gtk_dialog_run (GTK_DIALOG (dialog));
}

static void
create_main_dialog (NscConverter *converter)
{
	NscConverterPrivate *priv;
	GtkWidget           *hbox, *edit, *image;
	const gchar         *profile_id;
	gboolean             result;

	priv = NSC_CONVERTER_GET_PRIVATE (converter);

	/* Create the gtkbuilder and grab some widgets */
	result = nsc_xml_get_file ("main.xml",
				   "main_dialog", &priv->dialog,
				   "path_chooser", &priv->path_chooser,
				   "format_hbox", &hbox,
				   NULL);

	if (!result) {
		return;
	}

	/* Create the gstreamer audio profile chooser */
	priv->profile_chooser = gm_audio_profile_choose_new ();

	/* Set which profile is active */
	profile_id = gm_audio_profile_get_id (priv->profile);
	gm_audio_profile_choose_set_active (priv->profile_chooser,
					    profile_id);

	/* Create edit profile button */
	edit = gtk_button_new_with_mnemonic (_("Edit _Profiles..."));
	image = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_BUTTON);
	g_object_set (edit,
		      "gtk-button-images", TRUE,
		      NULL);
	gtk_button_set_image (GTK_BUTTON (edit), image);

	/* Let's pack the audio profile chooseer */
	gtk_box_pack_start (GTK_BOX (hbox), priv->profile_chooser,
			    FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), edit, FALSE, FALSE, 0);

	/* Connect signals */
	g_signal_connect (G_OBJECT (priv->dialog), "response",
			  (GCallback) converter_response_cb,
			  converter);
	g_signal_connect (G_OBJECT (edit), "clicked",
			  (GCallback) converter_edit_profile,
			  converter);
}

static void
nsc_converter_init (NscConverter *converter)
{
	NscConverterPrivate *priv;
	GConfClient         *gconf;

	priv = NSC_CONVERTER_GET_PRIVATE (converter);

	/* Set init values */
	priv->gst = NULL;
	priv->files_converted = 0;
	priv->current_duration = 0;
	priv->total_duration = 0;
	priv->before.seconds = -1;

	/* Get gconf client */
	gconf = gconf_client_get_default ();
	if (gconf == NULL) {
		/* Should probably do more than just give a warning */
		g_warning (_("Could not create GConf client.\n"));
	}

	/* Init gnome-media-profiles */
	gnome_media_profiles_init (gconf);

	/* Unreference the gconf client */
	g_object_unref (gconf);

	/* Set the profile to the default. */
	priv->profile = gm_audio_profile_lookup (DEFAULT_AUDIO_PROFILE_NAME);

	/* Create the dialog */
	create_main_dialog (converter);	
}

/*
 * Public Methods
 */
NscConverter *
nsc_converter_new (GList *files)
{
	return g_object_new (NSC_TYPE_CONVERTER, "files", files, NULL);
}

void
nsc_converter_show_dialog (NscConverter *converter)
{
	NscConverterPrivate *priv;

	priv = NSC_CONVERTER_GET_PRIVATE (converter);
	gtk_widget_show_all (priv->dialog);
}
