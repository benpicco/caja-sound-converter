AC_INIT([caja-sound-converter],[3.0.2],[https://bugzilla.gnome.org/browse.cgi?product=nautilus-sound-converter],[caja-sound-converter],[http://projects.gnome.org/nautilus-sound-converter/])
AC_PREREQ([2.68])
AC_COPYRIGHT([Copyright (C) 2008-2011 Brian Pepple])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.11.1 no-dist-gzip dist-xz tar-ustar check-news])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE([enable])

dnl -----------------------------------------------------------
dnl Checks for programs.
dnl -----------------------------------------------------------
AC_PROG_CC
IT_PROG_INTLTOOL([0.40.6])
LT_INIT
AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

AC_PATH_PROG(MATECONFTOOL, mateconftool-2)
AM_MATECONF_SOURCE_2

MATE_COMPILE_WARNINGS([maximum])
MATE_CXX_WARNINGS([yes])
MATE_DEBUG_CHECK
MATE_MAINTAINER_MODE_DEFINES

dnl -----------------------------------------------------------
dnl Set variables for minimum versions needed.
dnl -----------------------------------------------------------
GLIB_REQUIRED=2.16.0
CAJA_REQUIRED=1.1.0
MATECONF_REQUIRED=1.1.0
GTK_REQUIRED=2.24.0
GSTREAMER_REQUIRED=0.10.20
MATE_MEDIA_PROFILES_REQUIRED=1.1.0

dnl -----------------------------------------------------------
dnl Check for required libraries
dnl -----------------------------------------------------------
PKG_CHECK_MODULES(NSC,
[
	glib-2.0 >= $GLIB_REQUIRED
	mateconf-2.0 >= $MATECONF_REQUIRED
	libcaja-extension >= $CAJA_REQUIRED
	gtk+-2.0 >= $GTK_REQUIRED
	gstreamer-0.10 >= $GSTREAMER_REQUIRED
	libmate-media-profiles >= $MATE_MEDIA_PROFILES_REQUIRED
])
AC_SUBST(NSC_CFLAGS)
AC_SUBST(NSC_LIBS)

dnl -----------------------------------------------------------
dnl Get the correct caja extensions directory
dnl -----------------------------------------------------------
AC_ARG_WITH(cajadir,
	    AS_HELP_STRING([--with-cajadir=DIR],[Installation path for Caja extension @<:@auto@:>@]),
	    [ac_with_cajadir=$withval],
	    [ac_with_cajadir=""])
if test "${ac_with_cajadir}" = ""; then
	ac_with_cajadir=`pkg-config --variable=extensiondir libcaja-extension`
fi

AC_MSG_NOTICE([installing caja plugin in ${ac_with_cajadir}])
AC_SUBST([CAJA_EXTENSION_DIR],[${ac_with_cajadir}])


dnl -----------------------------------------------------------
dnl Find the GStreamer elements we use, and warn if they are
dnl not present.
dnl -----------------------------------------------------------
AM_GST_ELEMENT_CHECK(vorbisdec,,AC_MSG_WARN([The 'vorbisdec' element was not found. This will cause decoding from Ogg Vorbis to fail.]))
AM_GST_ELEMENT_CHECK(vorbisenc,,AC_MSG_WARN([The 'vorbisenc' element was not found. This will cause encoding to Ogg Vorbis to fail.]))
AM_GST_ELEMENT_CHECK(flacdec,,AC_MSG_WARN([The 'flacdec' element was not found. This will cause decoding from FLAC to fail.]))
AM_GST_ELEMENT_CHECK(flacenc,,AC_MSG_WARN([The 'flacenc' element was not found. This will cause encoding to FLAC to fail.]))
AM_GST_ELEMENT_CHECK(wavpackenc,,AC_MSG_WARN([The 'wavpackenc' element was not found. This will cause encoding from Wav to fail.]))
AM_GST_ELEMENT_CHECK(wavpackdec,,AC_MSG_WARN([The 'wavpackdec' element was not found. This will cause decoding to Wav to fail.]))
AM_GST_ELEMENT_CHECK(giosink,,AC_MSG_WARN([The 'giosink' element was not found. This will cause Caja-Sound-Converter to fail at runtime.]))
AM_GST_ELEMENT_CHECK(giosrc,,AC_MSG_WARN([The 'giosrc' element was not found. This will cause Caja-Sound-Converter to fail at runtime.]))
AM_GST_ELEMENT_CHECK(decodebin,,AC_MSG_WARN([The 'decodebin' element was not found. This will cause Caja-Sound-Converter to fail at runtime.]))
AM_GST_ELEMENT_CHECK(audioresample,,AC_MSG_WARN([The 'audioresample' element was not found.  This will cause Caja-Sound-Converter to fail at runtime.]))
AM_GST_ELEMENT_CHECK(audioconvert,,AC_MSG_WARN([The 'audioconvert' element was not found. This will cause Caja-Sound-Converter to fail at runtime.]))


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
