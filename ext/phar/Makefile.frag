
$(srcdir)/phar_path_check.c: $(srcdir)/phar_path_check.re
	$(RE2C) -b -o $(srcdir)/phar_path_check.c $(srcdir)/phar_path_check.re

pharcmd: $(builddir)/phar.php $(builddir)/phar.phar

$(builddir)/phar.php: $(srcdir)/build_precommand.php $(srcdir)/phar/*.inc $(srcdir)/phar/*.php $(SAPI_CLI_PATH)
	if test -x "$(PHP_EXECUTABLE)"; then \
		export PHP="$(PHP_EXECUTABLE)"; \
	else \
		export PHP="$(top_builddir)/$(SAPI_CLI_PATH)"; \
	fi; \
	$$PHP $(srcdir)/build_precommand.php > $(builddir)/phar.php

$(builddir)/phar.phar: $(builddir)/phar.php $(srcdir)/phar/*.inc $(srcdir)/phar/*.php $(SAPI_CLI_PATH)
	if test -x "$(PHP_EXECUTABLE)"; then \
		export PHP="$(PHP_EXECUTABLE)"; \
		export BANG="$(PHP_EXECUTABLE)"; \
	else \
		export PHP="$(top_builddir)/$(SAPI_CLI_PATH)"; \
		export BANG="$(INSTALL_ROOT)$(bindir)/$(program_prefix)php$(program_suffix)$(EXEEXT)"; \
	fi; \
	$$PHP -d phar.readonly=0 $(srcdir)/phar.php pack -f $(builddir)/phar.phar -a pharcommand -c auto -x CVS -p 0 -s $(srcdir)/phar/phar.php -h sha1 -b "$$BANG"  $(srcdir)/phar/
	@chmod +x $(builddir)/phar.phar

