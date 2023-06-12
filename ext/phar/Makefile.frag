$(srcdir)/phar_path_check.c: $(srcdir)/phar_path_check.re
	@(cd $(top_srcdir); \
	if test -f ./php_phar.h; then \
		$(RE2C) $(RE2C_FLAGS) --no-generation-date -b -o phar_path_check.c phar_path_check.re; \
	else \
		$(RE2C) $(RE2C_FLAGS) --no-generation-date -b -o ext/phar/phar_path_check.c ext/phar/phar_path_check.re; \
	fi)

pharcmd: $(builddir)/phar.php $(builddir)/phar.phar

PHP_PHARCMD_SETTINGS = -n -d 'open_basedir=' -d 'output_buffering=0' -d 'memory_limit=-1' -d phar.readonly=0
PHP_PHARCMD_EXECUTABLE = ` \
	if test -x "$(top_builddir)/$(SAPI_CLI_PATH)"; then \
		$(top_srcdir)/build/shtool echo -n -- "$(top_builddir)/$(SAPI_CLI_PATH) -n"; \
		if test "x$(PHP_MODULES)" != "x"; then \
		$(top_srcdir)/build/shtool echo -n -- " -d extension_dir=$(top_builddir)/modules"; \
		for i in bz2 zlib phar; do \
			if test -f "$(top_builddir)/modules/$$i.la"; then \
				. $(top_builddir)/modules/$$i.la; $(top_srcdir)/build/shtool echo -n -- " -d extension=$$dlname"; \
			fi; \
		done; \
		fi; \
	else \
		$(top_srcdir)/build/shtool echo -n -- "$(PHP_EXECUTABLE)"; \
	fi;`
PHP_PHARCMD_BANG = `$(top_srcdir)/build/shtool echo -n -- "$(INSTALL_ROOT)$(bindir)/$(program_prefix)php$(program_suffix)$(EXEEXT)";`

$(builddir)/phar/phar.inc: $(srcdir)/phar/phar.inc
	-@test -d $(builddir)/phar || mkdir $(builddir)/phar
	-@test -f $(builddir)/phar/phar.inc || cp $(srcdir)/phar/phar.inc $(builddir)/phar/phar.inc


TEST_PHP_EXECUTABLE = $(shell $(PHP_EXECUTABLE) -v 2>&1)
TEST_PHP_EXECUTABLE_RES = $(shell echo "$(TEST_PHP_EXECUTABLE)" | grep -c 'Exec format error')

$(builddir)/phar.php: $(srcdir)/build_precommand.php $(srcdir)/phar/*.inc $(srcdir)/phar/*.php $(SAPI_CLI_PATH)
	-@(echo "Generating phar.php"; \
	if [ "$(TEST_PHP_EXECUTABLE_RES)" != 1 ]; then \
		$(PHP_PHARCMD_EXECUTABLE) $(PHP_PHARCMD_SETTINGS) $(srcdir)/build_precommand.php > $(builddir)/phar.php; \
	else \
		echo "Skipping phar.php generating during cross compilation"; \
	fi)

$(builddir)/phar.phar: $(builddir)/phar.php $(builddir)/phar/phar.inc $(srcdir)/phar/*.inc $(srcdir)/phar/*.php $(SAPI_CLI_PATH)
	-@(echo "Generating phar.phar"; \
	if [ "$(TEST_PHP_EXECUTABLE_RES)" != 1 ]; then \
		rm -f $(builddir)/phar.phar; \
		rm -f $(srcdir)/phar.phar; \
		$(PHP_PHARCMD_EXECUTABLE) $(PHP_PHARCMD_SETTINGS) $(builddir)/phar.php pack -f $(builddir)/phar.phar -a pharcommand -c auto -x \\.svn -p 0 -s $(srcdir)/phar/phar.php -h sha1 -b "$(PHP_PHARCMD_BANG)"  $(srcdir)/phar/; \
		chmod +x $(builddir)/phar.phar; \
	else \
		echo "Skipping phar.phar generating during cross compilation"; \
	fi)

install-pharcmd: pharcmd
	@(if [ "$(TEST_PHP_EXECUTABLE_RES)" != 1 ]; then \
		$(mkinstalldirs) $(INSTALL_ROOT)$(bindir); \
		$(INSTALL) $(builddir)/phar.phar $(INSTALL_ROOT)$(bindir)/$(program_prefix)phar$(program_suffix).phar; \
		rm -f $(INSTALL_ROOT)$(bindir)/$(program_prefix)phar$(program_suffix); \
		$(LN_S) -f $(program_prefix)phar$(program_suffix).phar $(INSTALL_ROOT)$(bindir)/$(program_prefix)phar$(program_suffix); \
		$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1; \
		$(INSTALL_DATA) $(builddir)/phar.1 $(INSTALL_ROOT)$(mandir)/man1/$(program_prefix)phar$(program_suffix).1; \
		$(INSTALL_DATA) $(builddir)/phar.phar.1 $(INSTALL_ROOT)$(mandir)/man1/$(program_prefix)phar$(program_suffix).phar.1; \
	else \
		echo "Skipping install-pharcmd during cross compilation"; \
	fi)
