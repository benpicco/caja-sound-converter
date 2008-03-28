/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 *  nsc-converter.c
 * 
 *  Copyright (C) 2008 Brian Pepple
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
 * 
 */

#include <config.h> /* for GETTEXT_PACKAGE */

#include <string.h>

#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libnautilus-extension/nautilus-file-info.h>
#include <profiles/gnome-media-profiles.h>

#include "nsc-converter.h"

typedef struct _NscConverterPrivate NscConverterPrivate;

struct _NscConverterPrivate {
	GConfClient     *gconf;

	/* The current audio profile */
	GMAudioProfile *profile;
	
	/* Files to be convertered */
	GList		*files;
	gint		 total_files;

	GtkWidget	*dialog;
	GtkWidget	*path_chooser;
	GtkWidget       *profile_chooser;
};

/* Default profile name */
#define DEFAULT_AUDIO_PROFILE_NAME "cdlossy"

#define NSC_CONVERTER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), NSC_TYPE_CONVERTER, NscConverterPrivate))

G_DEFINE_TYPE (NscConverter, nsc_converter, G_TYPE_OBJECT)

enum {
	PROP_FILES = 1,
};

typedef enum {
	SIGNAL_TYPE_EXAMPLE,
	LAST_SIGNAL
} NscConverterSignalType;

static void
nsc_converter_finalize (GObject *object)
{
	NscConverter 	    *converter = NSC_CONVERTER (object);
	NscConverterPrivate *priv = NSC_CONVERTER_GET_PRIVATE (converter);

	/* Put clean-up code here */
	g_object_unref (priv->gconf);

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

	files_param_spec = g_param_spec_pointer ("files",
						 "Files",
						 "Set selected files",
						 G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_property (object_class,
					 PROP_FILES,
					 files_param_spec);
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

		/* Ok, let's get ready to rumble */
	}
	gtk_widget_destroy (dialog);
}

static void
converter_profile_changed (GtkWidget *widget,
			   gpointer   user_data)
{
	if (gtk_combo_box_get_active (GTK_COMBO_BOX (widget))!= -1) {
		GMAudioProfile *profile;

		profile = gm_audio_profile_choose_get_active (widget);
		/* TODO: save the change in gconf */
	}
}

/**
 * The Edit Profiles button was pressed.
 */
static void
converter_edit_profile (GtkButton *button,
			gpointer   user_data)
{
	NscConverterPrivate *priv;
	GtkWidget           *dialog;

	priv = NSC_CONVERTER_GET_PRIVATE (user_data);

	dialog = gm_audio_profiles_edit_new (priv->gconf,
					     GTK_WINDOW (priv->dialog));
	gtk_widget_show_all (dialog);
	gtk_dialog_run (GTK_DIALOG (dialog));
}

static void
create_main_dialog (NscConverter *converter)
{
	NscConverterPrivate *priv;
	GtkBuilder          *ui = NULL;
	GtkWidget           *hbox, *label, *edit, *image;
	gchar               *path;
	GError              *err = NULL;
	guint                result;

	priv = NSC_CONVERTER_GET_PRIVATE (converter);

	/* Let's create our gtkbuilder and load the xml file */
	ui = gtk_builder_new ();
	gtk_builder_set_translation_domain (ui, GETTEXT_PACKAGE);
	path = g_build_filename (DATADIR, PACKAGE, "main.xml", NULL);
	result = gtk_builder_add_from_file (ui, path, &err);
	g_free (path);

	/* If we're unable to load the xml file */
	if (result == 0) {
		g_warning ("Unable to load xml file: %s", err->message);
		g_error_free (err);
		return;
	}

	/* Grab some widgets */
	priv->dialog = GTK_WIDGET (gtk_builder_get_object (ui, "main_dialog"));
	priv->path_chooser = GTK_WIDGET (gtk_builder_get_object
					 (ui, "path_chooser"));

	/* Create the gstreamer audio profile chooser */
	priv->profile_chooser = gm_audio_profile_choose_new ();

	/* Create the output label */
	label = gtk_label_new (_("O_utput Format:"));
	gtk_label_set_use_underline (GTK_LABEL (label), TRUE);

	/* Create edit profile button */
	edit = gtk_button_new_with_mnemonic (_("Edit _Profiles"));
	image = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_BUTTON);

	g_object_set (edit,
		      "gtk-button-images", TRUE,
		      NULL);

	gtk_button_set_image (GTK_BUTTON (edit), image);

	/* Let's pack the widgets */
	hbox = GTK_WIDGET (gtk_builder_get_object (ui, "format_hbox"));
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (hbox), priv->profile_chooser,
			    TRUE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (hbox), edit, FALSE, FALSE, 5);

	/* Connect signals */
	g_signal_connect (G_OBJECT (priv->dialog), "response",
			  (GCallback) converter_response_cb,
			  converter);

	g_signal_connect (G_OBJECT (edit), "clicked",
			  (GCallback) converter_edit_profile,
			  converter);

	g_signal_connect (G_OBJECT (priv->profile_chooser), "changed",
			  (GCallback) converter_profile_changed,
			  converter);
}

static void
nsc_converter_init (NscConverter *converter)
{
	NscConverterPrivate *priv;

	priv = NSC_CONVERTER_GET_PRIVATE (converter);

	/* Get gconf client */
	priv->gconf = gconf_client_get_default ();
	if (priv->gconf == NULL) {
		/* Should probably do more than just give a warning */
		g_warning (_("Could not create GConf client.\n"));
	}

	/* Init gnome-media-profiles */
	gnome_media_profiles_init (priv->gconf);

	/* Set the profile to the default for now */
	priv->profile = gm_audio_profile_lookup (DEFAULT_AUDIO_PROFILE_NAME);

	/* Create the dialog */
	create_main_dialog (converter);	
}

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
