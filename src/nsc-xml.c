/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2002-2007 Imendio AB
 * Copyright (C) 2008 Brian Pepple
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 */

#include <config.h>

#include <gtk/gtk.h>

#include "nsc-xml.h"

static GtkBuilder *
xml_get_file (const gchar *filename,
              const gchar *first_object,
              va_list      args)
{
	GtkBuilder  *gui = NULL;
	const char  *name;
	gchar       *path;
	GObject    **object_ptr;
	GError      *err = NULL;

	/* Create the gtkbuilder */
	gui = gtk_builder_new ();
	gtk_builder_set_translation_domain (gui, GETTEXT_PACKAGE);
	path = g_build_filename (DATADIR, PACKAGE, filename, NULL);

	/* Load the xml file */
	if (!gtk_builder_add_from_file (gui, path, &err)) {
		g_critical ("XML file error: %s", err->message);
		g_clear_error (&err);
		g_free (path);
		g_object_unref (gui);

		/* we need to iterate and set all of the pointers to NULL */
		for (name = first_object; name; name = va_arg (args, const gchar *)) {
			object_ptr = va_arg (args, GObject**);

			*object_ptr = NULL;
		}
		return NULL;
	}
	g_free (path);

	/* Grab the widgets */
	for (name = first_object; name; name = va_arg (args, const gchar *)) {
		object_ptr = va_arg (args, GObject**);
		
		*object_ptr = gtk_builder_get_object (gui, name);
		
		if (!*object_ptr) {
			g_warning ("Widget '%s' at '%s' is missing.",
				   name, filename);
			continue;
		}
	}
	return gui;
}

GtkBuilder *
nsc_builder_get_file (const gchar *filename,
		      const gchar *first_object,
		      ...)
{
	GtkBuilder *gui;
	va_list     args;

	va_start (args, first_object);
	gui = xml_get_file (filename, first_object, args);
	va_end (args);

	return gui;
}
