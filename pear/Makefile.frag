# -*- makefile -*-

peardir=$(PEAR_INSTALLDIR)

# Skip all php.ini files altogether
PEAR_INSTALL_FLAGS = -n -dsafe_mode=0

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

