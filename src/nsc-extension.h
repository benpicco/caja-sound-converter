/*
 *  nsc-extension.h
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
 *  Author: Brian Pepple <bdpepple@fedoraproject.org>
 * 
 */

#ifndef NSC_EXTENSION_H
#define NSC_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

/*
 * Declarations for the nautilus extension object.  This object will be
 * instantiated by nautilus.  It implements the GInterfaces exported by libnautilus.
 */

#define NSC_TYPE_EXTENSION	   (nsc_extension_get_type ())
#define NSC_EXTENSION(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), NSC_TYPE_EXTENSION, NscExtension))
#define NSC_IS_EXTENSION(o)	   (G_TYPE_CHECK_INSTANCE_TYPE ((o), NSC_TYPE_EXTENSION))

typedef struct _NscExtension	   NscExtension;
typedef struct _NscExtensionClass  NscExtensionClass;

struct _NscExtension {
	GObject parent_slot;
};

struct _NscExtensionClass {
	GObjectClass parent_slot;
};

GType nsc_extension_get_type      (void);
void  nsc_extension_register_type (GTypeModule *module);

G_END_DECLS

#endif
