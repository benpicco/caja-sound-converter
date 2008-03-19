/*
 *  nsc-converter.h
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

#ifndef NSC_CONVERTER_H
#define NSC_CONVERTER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define NSC_TYPE_CONVERTER         (nsc_converter_get_type ())
#define NSC_CONVERTER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), NSC_TYPE_CONVERTER, NscConverter))
#define NSC_CONVERTER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), NSC_TYPE_CONVERTER, NscConverterClass))
#define NSC_IS_CONVERTER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), NSC_TYPE_CONVERTER))
#define NSC_IS_CONVERTER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), NSC_TYPE_CONVERTER))
#define NSC_CONVERTER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), NSC_TYPE_CONVERTER, NscConverterClass))

typedef struct _NscConverter      NscConverter;
typedef struct _NscConverterClass NscConverterClass;

struct _NscConverter {
	GObject parent;
};

struct _NscConverterClass {
	GObjectClass parent_class;
};

GType		 nsc_converter_get_type    (void);
NscConverter	*nsc_converter_new 	   (GList *files);
void		 nsc_converter_show_dialog (NscConverter *dialog);

G_END_DECLS

#endif /* NSC_CONVERTER_H */
