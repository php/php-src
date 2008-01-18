phpincludedir=$(prefix)/include/php

ZIP_HEADER_FILES= \
	zip.h \
	zipint.h \
	zip_alias.h \
	zipint_alias.h

install-zip-headers:
	@echo "Installing ZIP headers:          $(INSTALL_ROOT)$(phpincludedir)/ext/zip/lib"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(phpincludedir)/ext/zip/lib
	@for f in $(ZIP_HEADER_FILES); do \
		if test -f "$(top_srcdir)/lib/$$f"; then \
			$(INSTALL_DATA) $(top_srcdir)/lib/$$f $(INSTALL_ROOT)$(phpincludedir)/ext/zip/lib; \
		elif test -f "$(top_builddir)/lib/$$f"; then \
			$(INSTALL_DATA) $(top_builddir)/lib/$$f $(INSTALL_ROOT)$(phpincludedir)/ext/zip/lib; \
		elif test -f "$(top_srcdir)/ext/zip/lib/$$f"; then \
			$(INSTALL_DATA) $(top_srcdir)/ext/zip/lib/$$f $(INSTALL_ROOT)$(phpincludedir)/ext/zip/lib; \
		elif test -f "$(top_builddir)/ext/zip/lib/$$f"; then \
			$(INSTALL_DATA) $(top_builddir)/ext/zip/lib/$$f $(INSTALL_ROOT)$(phpincludedir)/ext/zip/lib; \
		else \
			echo "hmmm"; \
		fi \
	done;

# mini hack
install: $(all_targets) $(install_targets) install-zip-headers

