AM_CPPFLAGS =						\
	-DG_LOG_DOMAIN=\"Nautilus-Sound-Converter\"	\
	-DDATADIR=\"$(datadir)\"			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	-Werror                                         \
	$(NSC_CFLAGS)

nautilus_extensiondir=$(NAUTILUS_EXTENSION_DIR)

nautilus_extension_LTLIBRARIES=libnautilus-sound-converter.la

libnautilus_sound_converter_la_SOURCES =	\
	nsc-module.c				\
	nsc-extension.c		nsc-extension.h	\
	nsc-converter.c		nsc-converter.h

libnautilus_sound_converter_la_LDFLAGS = -module -avoid-version
libnautilus_sound_converter_la_LIBADD  = $(NSC_LIBS)
