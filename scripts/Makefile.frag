
#
# Build environment install
#

phpincludedir = $(includedir)/php
phpbuilddir = $(prefix)/lib/php/build

BUILD_FILES = \
	scripts/phpize.m4 \
	build/mkdep.awk \
	build/shtool \
	build/scan_makefile_in.awk \
	Makefile.global \
	acinclude.m4

bin_SCRIPTS = phpize php-config
bin_src_SCRIPTS = phpextdist

install-build:
	@echo "Installing build environment:     $(INSTALL_ROOT)$(phpbuilddir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(phpbuilddir) $(INSTALL_ROOT)$(bindir) && \
	(cd $(top_srcdir) && $(INSTALL) $(BUILD_FILES) $(INSTALL_ROOT)$(phpbuilddir))

HEADER_DIRS = \
	/ \
	Zend/ \
	TSRM/ \
	main/ \
	main/streams/ \
	regex/ \
	ext/standard/ \
	ext/session/ \
	ext/xml/ \
	ext/xml/expat/ \
	ext/mbstring/ \
	ext/sqlite/libsqlite/src/sqlite.h

install-headers:
	-@for i in $(HEADER_DIRS); do \
		i=`$(top_srcdir)/build/shtool path -d $$i`; \
		paths="$$paths $(INSTALL_ROOT)$(phpincludedir)/$$i"; \
	done; \
	$(mkinstalldirs) $$paths && \
	echo "Installing header files:          $(INSTALL_ROOT)$(phpincludedir)/" && \
	for i in $(HEADER_DIRS); do \
		if test -f "$(top_srcdir)/$$i"; then \
			$(INSTALL_DATA) $(top_srcdir)/$$i $(INSTALL_ROOT)$(phpincludedir)/$$i; \
		elif test -f "$(top_builddir)/$$i"; then \
			$(INSTALL_DATA) $(top_builddir)/$$i $(INSTALL_ROOT)$(phpincludedir)/$$i; \
		else \
			(cd $(top_srcdir)/$$i && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/$$i; \
			cd $(top_builddir)/$$i && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/$$i) 2>/dev/null || true; \
		fi \
	done; \
	cd $(top_srcdir)/sapi/embed && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/main

install-programs: $(builddir)/phpize $(builddir)/php-config
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
