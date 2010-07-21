/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 *  nsc-module.c
 * 
 *  Copyright (C) 2007-2008 Brian Pepple
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

#include "nsc-extension.h"

#include <libintl.h>

static GType type_list[1];

void nautilus_module_initialize (GTypeModule  *module);
void nautilus_module_shutdown   (void);
void nautilus_module_list_types (const GType **types,
				 int          *num_types);

void
nautilus_module_initialize (GTypeModule *module)
{
	g_print ("Initializing nautilus-sound-converter extension\n");

	nsc_extension_register_type (module);
	type_list[0] = NSC_TYPE_EXTENSION;

	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
}

void
nautilus_module_shutdown (void)
{
	g_print ("Shutting down nautilus-sound-converter extension\n");
}

void 
nautilus_module_list_types (const GType **types,
			    int          *num_types)
{
	*types = type_list;
	*num_types = G_N_ELEMENTS (type_list);
}
