
#
# Build environment install
#

phpincludedir = $(includedir)/php
phpbuilddir = $(prefix)/lib/php/build

BUILD_FILES = \
	scripts/phpize.m4 \
	build/mkdep.awk \
	build/shtool \
	Makefile.global \
	scan_makefile_in.awk \
	acinclude.m4

bin_SCRIPTS = phpize php-config
bin_src_SCRIPTS = phpextdist

install-build:
	@echo "Installing build environment:     $(INSTALL_ROOT)$(phpbuilddir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(phpbuilddir) $(INSTALL_ROOT)$(bindir) && \
	(cd $(top_srcdir) && cp $(BUILD_FILES) $(INSTALL_ROOT)$(phpbuilddir))

HEADER_DIRS = \
	/ \
	Zend \
	TSRM \
	ext/standard \
	ext/session \
	ext/xml \
	ext/xml/expat \
	main \
	main/streams \
	ext/mbstring \
	ext/pgsql \
	regex

install-headers:
	-@for i in $(HEADER_DIRS); do \
		paths="$$paths $(INSTALL_ROOT)$(phpincludedir)/$$i"; \
	done; \
	$(mkinstalldirs) $$paths && \
	echo "Installing header files:          $(INSTALL_ROOT)$(phpincludedir)/" && \
	for i in $(HEADER_DIRS); do \
		(cd $(top_srcdir)/$$i && cp -p *.h $(INSTALL_ROOT)$(phpincludedir)/$$i; \
		cd $(top_builddir)/$$i && cp -p *.h $(INSTALL_ROOT)$(phpincludedir)/$$i) 2>/dev/null || true; \
	done; \
	cd $(top_srcdir)/sapi/embed && cp -p *.h $(INSTALL_ROOT)$(phpincludedir)/main

install-programs:
	@echo "Installing helper programs:       $(INSTALL_ROOT)$(bindir)/"
	@for prog in $(bin_SCRIPTS); do \
		echo "  program: $$prog"; \
		$(INSTALL) -m 755 $(builddir)/$$prog $(INSTALL_ROOT)$(bindir)/$$prog; \
	done
	@for prog in $(bin_src_SCRIPTS); do \
		echo "  program: $$prog"; \
		$(INSTALL) -m 755 $(top_srcdir)/scripts/$$prog $(INSTALL_ROOT)$(bindir)/$$prog; \
	done

$(builddir)/phpize: $(srcdir)/phpize.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

$(builddir)/php-config: $(srcdir)/php-config.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)
