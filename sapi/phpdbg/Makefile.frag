phpdbg: $(BUILD_BINARY)

phpdbg-shared: $(BUILD_SHARED)

$(BUILD_SHARED): $(PHP_GLOBAL_OBJS) $(PHP_BINARY_OBJS) $(PHP_PHPDBG_OBJS)
	$(BUILD_PHPDBG_SHARED)

$(BUILD_BINARY): $(PHP_GLOBAL_OBJS) $(PHP_BINARY_OBJS) $(PHP_PHPDBG_OBJS)
	$(BUILD_PHPDBG)

%.c: %.y
%.c: %.l

$(builddir)/phpdbg_lexer.lo: $(srcdir)/phpdbg_parser.h

$(srcdir)/phpdbg_lexer.c: $(srcdir)/phpdbg_lexer.l
	@(cd $(top_srcdir); $(RE2C) $(RE2C_FLAGS) --no-generation-date -cbdFo sapi/phpdbg/phpdbg_lexer.c sapi/phpdbg/phpdbg_lexer.l)

$(srcdir)/phpdbg_parser.h: $(srcdir)/phpdbg_parser.c
$(srcdir)/phpdbg_parser.c: $(srcdir)/phpdbg_parser.y
	@$(YACC) -p phpdbg_ -v -d $(srcdir)/phpdbg_parser.y -o $@

install-phpdbg: $(BUILD_BINARY)
	@echo "Installing phpdbg binary:         $(INSTALL_ROOT)$(bindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/log
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/run
	@$(INSTALL) -m 0755 $(BUILD_BINARY) $(INSTALL_ROOT)$(bindir)/$(program_prefix)phpdbg$(program_suffix)$(EXEEXT)
	@echo "Installing phpdbg man page:       $(INSTALL_ROOT)$(mandir)/man1/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1
	@$(INSTALL_DATA) sapi/phpdbg/phpdbg.1 $(INSTALL_ROOT)$(mandir)/man1/$(program_prefix)phpdbg$(program_suffix).1

clean-phpdbg:
	@echo "Cleaning phpdbg object files ..."
	find sapi/phpdbg/ -name *.lo -o -name *.o | xargs rm -f

test-phpdbg:
	@echo "Running phpdbg tests ..."
	@$(top_builddir)/sapi/cli/php sapi/phpdbg/tests/run-tests.php --phpdbg sapi/phpdbg/phpdbg

.PHONY: clean-phpdbg test-phpdbg
