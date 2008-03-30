/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 *  nsc-progress.c
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

#include <config.h>

#include <gtk/gtkbuilder.h>
#include <gtk/gtkdialog.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkprogressbar.h>

#include "nsc-progress.h"
#include "nsc-xml.h"

#define XML_FILE "progress.xml"

#define GET_PRIV(obj)                \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), NSC_TYPE_PROGRESS, NscProgressPriv))

typedef struct {
	GtkWidget *dialog;
	GtkWidget *label;
	GtkWidget *progressbar;
} NscProgressPriv;

static void nsc_progress_finalize (GObject   *object);
static void dialog_response_cb    (GtkWidget *dialog,
				   gint       response_id,
				   gpointer   user_data);

G_DEFINE_TYPE (NscProgress, nsc_progress, G_TYPE_OBJECT);

static void
nsc_progress_class_init (NscProgressClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = nsc_progress_finalize;

	g_type_class_add_private (object_class, sizeof (NscProgressPriv));
}

static void
nsc_progress_init (NscProgress *progress)
{
	NscProgressPriv *priv;
	GtkBuilder      *ui = NULL;

	priv = GET_PRIV (progress);

	/* Create the gtkbuilder, and grab the widgets */
	ui = nsc_xml_get_file (XML_FILE,
			       "progress_dialog", &priv->dialog,
			       "progress_label", &priv->label,
			       "file_progressbar", &priv->progressbar,
			       NULL);
	
	/* Connect the signals */
	nsc_xml_connect (ui,
			 progress,
			 "progress_dialog", "response", dialog_response_cb,
			 NULL);
	
	g_object_unref (ui);
}

static void
nsc_progress_finalize (GObject *object)
{
	NscProgressPriv *priv;

	priv = GET_PRIV (object);

	G_OBJECT_CLASS (nsc_progress_parent_class)->finalize (object);
}

static void
dialog_response_cb (GtkWidget *dialog,
		    gint       response_id,
		    gpointer   user_data)
{
	if (response_id == GTK_RESPONSE_CANCEL) {
		/* TODO: Add code to cancel gst action */
	}
	gtk_widget_destroy (dialog);
}

void
nsc_progress_show (void)
{
	NscProgress *progress;

	progress = g_object_new (NSC_TYPE_PROGRESS, NULL);
}
