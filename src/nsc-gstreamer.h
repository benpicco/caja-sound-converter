/*
 *  nsc-gstreamer.h
 * 
 *  Copyright (C) 2008 Brian Pepple
 *  Copyright (C) 2003 Ross Burton <ross@burtonini.com>
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
 *  Author: Ross Burton <ross@burtonini.com>
 *          Brian Pepple <bpepple@fedoraproject.org>
 * 
 */

#ifndef NSC_GSTREAMER_H
#define NSC_GSTREAMER_H

#include <glib/gmacros.h>
#include <glib-object.h>
#include <profiles/audio-profile.h>

G_BEGIN_DECLS

#define NSC_TYPE_GSTREAMER            (nsc_gstreamer_get_type ())
#define NSC_GSTREAMER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NSC_TYPE_GSTREAMER, NscGStreamer))
#define NSC_GSTREAMER_CLASS(klass)    (G_TYPE_CHECK_INSTANCE_TYPE (obj, NSC_TYPE_GSTREAMER, NscGStreamerClass))
#define NSC_IS_GSTREAMER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE (obj, NSC_TYPE_GSTREAMER))
#define NSC_IS_GSTREAMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NSC_TYPE_GSTREAMER))
#define NSC_GSTREAMER_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), NSC_TYPE_GSTREAMER, NscGStreamerClass))

typedef struct NscGStreamerPrivate NscGStreamerPrivate;

typedef struct {
	GObject              object;
	NscGStreamerPrivate *priv;
} NscGStreamer;

typedef struct {
	GObjectClass        parent_class;
	void (*progress)   (NscGStreamer *gstreamer, const int seconds);
	void (*duration)   (NscGStreamer *gstreamer, const int seconds);
	void (*completion) (NscGStreamer *gstreamer);
	void (*error)      (NscGStreamer *gstreamer, GError *error);
} NscGStreamerClass;

GType         nsc_gstreamer_get_type          (void);

NscGStreamer *nsc_gstreamer_new               (GMAudioProfile  *profile);
GError       *nsc_gstreamer_get_new_error     (NscGStreamer    *gstreamer);

void          nsc_gstreamer_convert_file      (NscGStreamer    *gstreamer,
					       const gchar     *src_url,
					       const gchar     *sink_url,
					       GError         **error);
void          nsc_gstreamer_cancel_convert    (NscGStreamer    *gstreamer);

gboolean      nsc_gstreamer_supports_profile  (GMAudioProfile  *profile);
gboolean      nsc_gstreamer_supports_mp3      (GError         **error);
gboolean      nsc_gstreamer_supports_wav      (GError         **error);
gboolean      nsc_gstreamer_supports_aac      (GError         **error);

G_END_DECLS

#endif /* NSC_GSTREAMER_H */
