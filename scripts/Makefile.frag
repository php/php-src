
#
# Build environment install
#

phpincludedir = $(includedir)/php
phpbuilddir = $(prefix)/lib/php/build

BUILD_FILES = \
	scripts/phpize.m4 \
	build/mkdep.awk \
	build/scan_makefile_in.awk \
	build/libtool.m4 \
	Makefile.global \
	acinclude.m4 \
	ltmain.sh

BUILD_FILES_EXEC = \
	build/shtool \
	config.guess \
	config.sub

bin_SCRIPTS = phpize php-config
bin_src_SCRIPTS = phpextdist

install-build:
	@echo "Installing build environment:     $(INSTALL_ROOT)$(phpbuilddir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(phpbuilddir) $(INSTALL_ROOT)$(bindir) && \
	(cd $(top_srcdir) && \
	$(INSTALL) $(BUILD_FILES_EXEC) $(INSTALL_ROOT)$(phpbuilddir) && \
	$(INSTALL_DATA) $(BUILD_FILES) $(INSTALL_ROOT)$(phpbuilddir))

HEADER_DIRS = \
	/ \
	Zend \
	TSRM \
	ext/standard \
	ext/session \
	ext/xml \
	ext/xml/expat \
	main \
	ext/mbstring \
	ext/mbstring/libmbfl \
	ext/mbstring/libmbfl/mbfl \
	ext/pgsql \
	regex

install-headers:
	-@for i in $(HEADER_DIRS); do \
		paths="$$paths $(INSTALL_ROOT)$(phpincludedir)/$$i"; \
	done; \
	$(mkinstalldirs) $$paths && \
	echo "Installing header files:          $(INSTALL_ROOT)$(phpincludedir)/" && \
	for i in $(HEADER_DIRS); do \
		(cd $(top_srcdir)/$$i && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/$$i; \
		cd $(top_builddir)/$$i && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/$$i) 2>/dev/null || true; \
	done; \
	cd $(top_srcdir)/sapi/embed && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/main

install-programs: $(builddir)/phpize $(builddir)/php-config 
	@echo "Installing helper programs:       $(INSTALL_ROOT)$(bindir)/"
	@for prog in $(bin_SCRIPTS); do \
		echo "  program: $(program_prefix)$$prog$(program_suffix)"; \
		$(INSTALL) -m 755 $(builddir)/$$prog $(INSTALL_ROOT)$(bindir)/$(program_prefix)$$prog$(program_suffix); \
	done
	@for prog in $(bin_src_SCRIPTS); do \
		echo "  program: $(program_prefix)$$prog$(program_suffix)"; \
		$(INSTALL) -m 755 $(top_srcdir)/scripts/$$prog $(INSTALL_ROOT)$(bindir)/$(program_prefix)$$prog$(program_suffix); \
	done

$(builddir)/phpize: $(srcdir)/phpize.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

$(builddir)/php-config: $(srcdir)/php-config.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)
