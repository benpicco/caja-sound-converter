/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 *  nsc-progress.h
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

#ifndef NSC_PROGRESS_H
#define NSC_PROGRESS_H

#include <glib-object.h>

G_BEGIN_DECLS

#define NSC_TYPE_PROGRESS         (nsc_progress_get_type ())
#define NSC_PROGRESS(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), NSC_TYPE_PROGRESS, NscProgress))
#define NSC_PROGRESS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), NSC_TYPE_PROGRESS, NscProgressClass))
#define NSC_IS_PROGRESS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), NSC_TYPE_PROGRESS))
#define NSC_IS_PROGRESS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), NSC_TYPE_PROGRESS))
#define NSC_PROGRESS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), NSC_TYPE_PROGRESS, NscProgressClass))

typedef struct _NscProgress      NscProgress;
typedef struct _NscProgressClass NscProgressClass;

struct _NscProgress {
	GObject parent;
};

struct _NscProgressClass {
	GObjectClass parent_class;
};

GType            nsc_progress_get_type           (void) G_GNUC_CONST;
void             nsc_progress_show               (void);

G_END_DECLS

#endif /* NSC_PROGRESS_H */
