phpincludedir=$(prefix)/include/php

PDO_HEADER_FILES= \
	php_pdo.h \
	php_pdo_driver.h

install-pdo-headers:
	echo "Installing PDO headers:          $(INSTALL_ROOT)$(phpincludedir)/ext/pdo/"
	$(mkinstalldirs) $(INSTALL_ROOT)$(phpincludedir)/ext/pdo
	for f in $(PDO_HEADER_FILES); do \
		if test -f "$(top_srcdir)/$$f"; then \
			$(INSTALL_DATA) $(top_srcdir)/$$f $(INSTALL_ROOT)$(phpincludedir)/ext/pdo; \
		elif test -f "$(top_builddir)/$$f"; then \
			$(INSTALL_DATA) $(top_builddir)/$$f $(INSTALL_ROOT)$(phpincludedir)/ext/pdo; \
		elif test -f "$(top_srcdir)/ext/pdo/$$f"; then \
			$(INSTALL_DATA) $(top_srcdir)/ext/pdo/$$f $(INSTALL_ROOT)$(phpincludedir)/ext/pdo; \
		elif test -f "$(top_builddir)/ext/pdo/$$f"; then \
			$(INSTALL_DATA) $(top_builddir)/ext/pdo/$$f $(INSTALL_ROOT)$(phpincludedir)/ext/pdo; \
		else \
			echo "hmmm"; \
		fi \
	done;
	@echo ""
	@echo ""
	@echo "You're now ready to install PDO drivers for your database"
	@echo "If you'd like to have PDO show up in the list of installed"
	@echo "packages when you run \"pear list\", you may now run"
	@echo "\"sudo pear install package.xml\""
	@echo "this time, when prompted to press enter, you shold type"
	@echo "\"no\" and press enter instead."

# mini hack
install: $(all_targets) $(install_targets) install-pdo-headers

$(top_srcdir)/ext/pdo/pdo_sql_parser.c: $(top_srcdir)/ext/pdo/pdo_sql_parser.re
	exit 0; re2c -b $(top_srcdir)/ext/pdo/pdo_sql_parser.re > $@

$(srcdir)/pdo_sql_parser.c: $(srcdir)/pdo_sql_parser.re
	exit 0; re2c -b $(srcdir)/pdo_sql_parser.re > $@
