AC_INIT([nautilus-sound-converter], [1.0.1], [http://code.google.com/p/nautilus-sound-converter/issues/entry], [nautilus-sound-converter])
AC_PREREQ([2.59])
AC_COPYRIGHT([Copyright (C) 2008-2009 Brian Pepple])

AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.9 dist-bzip2])

AM_MAINTAINER_MODE

dnl -----------------------------------------------------------
dnl Checks for programs.
dnl -----------------------------------------------------------
AC_PROG_CC
IT_PROG_INTLTOOL([0.35.0])
AC_PROG_LIBTOOL
AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

GNOME_COMPILE_WARNINGS([maximum])
GNOME_CXX_WARNINGS
GNOME_DEBUG_CHECK
GNOME_MAINTAINER_MODE_DEFINES

dnl -----------------------------------------------------------
dnl Set variables for minimum versions needed.
dnl -----------------------------------------------------------
GLIB_REQUIRED=2.16.0
NAUTILUS_REQUIRED=2.12.0
GCONF_REQUIRED=1.2.0
GTK_REQUIRED=2.12.0
GSTREAMER_REQUIRED=0.10.20
GNOME_MEDIA_PROFILES_REQUIRED=2.11.91

dnl -----------------------------------------------------------
dnl Check for required libraries
dnl -----------------------------------------------------------
PKG_CHECK_MODULES(NSC,
[
	glib-2.0 >= $GLIB_REQUIRED
	gconf-2.0 >= $GCONF_REQUIRED
	libnautilus-extension >= $NAUTILUS_REQUIRED
	gtk+-2.0 >= $GTK_REQUIRED
	gstreamer-0.10 >= $GSTREAMER_REQUIRED
	gnome-media-profiles >= $GNOME_MEDIA_PROFILES_REQUIRED
])
AC_SUBST(NSC_CFLAGS)
AC_SUBST(NSC_LIBS)

dnl -----------------------------------------------------------
dnl Get the correct nautilus extensions directory
dnl -----------------------------------------------------------
NAUTILUS_EXTENSION_DIR=`$PKG_CONFIG --variable=extensiondir libnautilus-extension`
AC_SUBST(NAUTILUS_EXTENSION_DIR)

dnl -----------------------------------------------------------
dnl Find the GStreamer elements we use, and warn if they are
dnl not present.
dnl -----------------------------------------------------------
AM_GST_ELEMENT_CHECK(vorbisdec,,AC_MSG_WARN([The 'vorbisdec' element was not found. This will cause decoding from Ogg Vorbis to fail.]))
AM_GST_ELEMENT_CHECK(vorbisenc,,AC_MSG_WARN([The 'vorbisenc' element was not found. This will cause encoding to Ogg Vorbis to fail.]))
AM_GST_ELEMENT_CHECK(flacdec,,AC_MSG_WARN([The 'flacdec' element was not found. This will cause decoding from FLAC to fail.]))
AM_GST_ELEMENT_CHECK(flacenc,,AC_MSG_WARN([The 'flacenc' element was not found. This will cause encoding to FLAC to fail.]))
AM_GST_ELEMENT_CHECK(giosink,,AC_MSG_WARN([The 'giosink' element was not found. This will cause Nautilus-Sound-Converter to fail at runtime.]))
AM_GST_ELEMENT_CHECK(giosrc,,AC_MSG_WARN([The 'giosrc' element was not found. This will cause Nautilus-Sound-Converter to fail at runtime.]))
AM_GST_ELEMENT_CHECK(decodebin,,AC_MSG_WARN([The 'decodebin' element was not found. This will cause Nautilus-Sound-Converter to fail at runtime.]))
AM_GST_ELEMENT_CHECK(audioresample,,AC_MSG_WARN([The 'audioresample' element was not found.  This will cause Nautilus-Sound-Converter to fail at runtime.]))
AM_GST_ELEMENT_CHECK(audioconvert,,AC_MSG_WARN([The 'audioconvert' element was not found. This will cause Nautilus-Sound-Converter to fail at runtime.]))


dnl -----------------------------------------------------------
dnl Language Support
dnl -----------------------------------------------------------
GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

dnl -----------------------------------------------------------
dnl Write Makefiles
dnl -----------------------------------------------------------
AC_CONFIG_FILES([
	Makefile
	data/Makefile
	src/Makefile
	po/Makefile.in
])

AC_OUTPUT

echo
echo $PACKAGE $VERSION
echo
echo "Prefix:   $prefix"
