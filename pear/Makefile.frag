# -*- makefile -*-

pear_install_targets = \
	install-pear \
	install-headers \
	install-build \
	install-programs

peardir=$(PEAR_INSTALLDIR)

PEAR_INSTALL_FLAGS = \
	-d 'safe_mode=0' \
	-d 'open_basedir=' \

install-pear-installer: $(top_builddir)/sapi/cli/php
	@$(top_builddir)/sapi/cli/php $(PEAR_INSTALL_FLAGS) $(srcdir)/install-pear.php $(srcdir)/package-*.xml

install-pear-packages: $(top_builddir)/sapi/cli/php
	@$(top_builddir)/sapi/cli/php $(PEAR_INSTALL_FLAGS) $(srcdir)/install-pear.php $(srcdir)/packages/*.tar

install-pear:
	@echo "Installing PEAR environment:      $(INSTALL_ROOT)$(peardir)/"
	@if $(mkinstalldirs) $(INSTALL_ROOT)$(peardir); then \
		$(MAKE) -s install-pear-installer install-pear-packages; \
	else \
		cat $(srcdir)/install-pear.txt; \
		exit 5; \
	fi

phpincludedir = $(includedir)/php
phpbuilddir = $(prefix)/lib/php/build

BUILD_FILES = \
	pear/pear.m4 \
	build/mkdep.awk \
	build/shtool \
	Makefile.global \
	scan_makefile_in.awk \
	acinclude.m4

bin_SCRIPTS = phpize php-config phpextdist

install-build:
	@echo "Installing build environment:     $(INSTALL_ROOT)$(phpbuilddir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(phpbuilddir) $(INSTALL_ROOT)$(bindir) && \
	(cd $(top_srcdir) && cp $(BUILD_FILES) $(INSTALL_ROOT)$(phpbuilddir))

install-programs:
	@echo "Installing helper programs:       $(INSTALL_ROOT)$(bindir)/"
	@for prog in $(bin_SCRIPTS); do \
		echo "  program: $$prog"; \
		$(INSTALL) -m 755 $(builddir)/scripts/$$prog $(INSTALL_ROOT)$(bindir)/$$prog; \
	done

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

#$(builddir)/scripts/pear: $(srcdir)/scripts/pear.in $(top_builddir)/config.status
#	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

$(builddir)/scripts/phpize: $(srcdir)/scripts/phpize.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

#$(builddir)/scripts/phptar: $(srcdir)/scripts/phptar.in $(top_builddir)/config.status
#	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

$(builddir)/scripts/php-config: $(srcdir)/scripts/php-config.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)
