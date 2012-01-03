AM_CPPFLAGS =						\
	-DG_LOG_DOMAIN=\"Caja-Sound-Converter\"	\
	-DDATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(NSC_CFLAGS) $(WARN_CFLAGS)

caja_extensiondir=$(CAJA_EXTENSION_DIR)

nautilus_extension_LTLIBRARIES=libnautilus-sound-converter.la

libnautilus_sound_converter_la_SOURCES =		\
	nsc-module.c					\
	nsc-error.c		nsc-error.h		\
	nsc-extension.c		nsc-extension.h		\
	nsc-converter.c		nsc-converter.h		\
	nsc-gstreamer.c		nsc-gstreamer.h		\
	nsc-xml.c		nsc-xml.h

libnautilus_sound_converter_la_LDFLAGS = -module -avoid-version
libnautilus_sound_converter_la_LIBADD  = $(NSC_LIBS)
