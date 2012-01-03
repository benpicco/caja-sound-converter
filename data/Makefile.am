schemasdir	 = $(MATECONF_SCHEMA_FILE_DIR)
schemas_in_files = caja-sound-converter.schemas.in
schemas_DATA 	 = $(schemas_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

builderdir = $(datadir)/caja-sound-converter
builder_DATA =		\
	main.ui		\
	progress.ui
	
EXTRA_DIST =			\
	$(builder_DATA)		\
	$(schemas_in_files)

DISTCLEANFILES =		\
	$(schemas_DATA)

if MATECONF_SCHEMAS_INSTALL
install-data-local:
	        if test -z "$(DESTDIR)" ; then \
	                for p in $(schemas_DATA) ; do \
	                        MATECONF_CONFIG_SOURCE=$(MATECONF_SCHEMA_CONFIG_SOURCE) $(MATECONFTOOL) --makefile-install-rule $(top_builddir)/data/$$p ; \
	                done \
	        fi
else
install-data-local:
endif
