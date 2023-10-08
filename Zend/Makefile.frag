#
# Zend
#

$(builddir)/zend_language_scanner.lo: $(srcdir)/zend_language_parser.h
$(builddir)/zend_ini_scanner.lo: $(srcdir)/zend_ini_parser.h

$(srcdir)/zend_language_scanner.c $(srcdir)/zend_language_scanner_defs.h: $(srcdir)/zend_language_scanner.l
	@(cd $(top_srcdir); $(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt Zend/zend_language_scanner_defs.h -oZend/zend_language_scanner.c Zend/zend_language_scanner.l)

$(srcdir)/zend_language_parser.h: $(srcdir)/zend_language_parser.c
$(srcdir)/zend_language_parser.c: $(srcdir)/zend_language_parser.y
# Tweak zendparse to be exported through ZEND_API. This has to be revisited once
# bison supports foreign skeletons and that bison version is used. Read
# https://git.savannah.gnu.org/cgit/bison.git/tree/data/README.md for more.
	@$(YACC) $(YFLAGS) -v -d $(srcdir)/zend_language_parser.y -o $@
	@$(SED) -e 's,^int zendparse\(.*\),ZEND_API int zendparse\1,g' < $@ \
	> $@.tmp && \
	mv $@.tmp $@
	@$(SED) -e 's,^int zendparse\(.*\),ZEND_API int zendparse\1,g' < $(srcdir)/zend_language_parser.h \
	> $(srcdir)/zend_language_parser.h.tmp && \
	mv $(srcdir)/zend_language_parser.h.tmp $(srcdir)/zend_language_parser.h

$(srcdir)/zend_ini_parser.h: $(srcdir)/zend_ini_parser.c
$(srcdir)/zend_ini_parser.c: $(srcdir)/zend_ini_parser.y
	$(YACC) $(YFLAGS) -v -d $(srcdir)/zend_ini_parser.y -o $@

$(srcdir)/zend_ini_scanner.c: $(srcdir)/zend_ini_scanner.l
	@(cd $(top_srcdir); $(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt Zend/zend_ini_scanner_defs.h -oZend/zend_ini_scanner.c Zend/zend_ini_scanner.l)

# Use an intermediate target to indicate that zend_vm_gen.php produces both files
# at the same time, rather than the same recipe applying for two different targets.
# The "grouped targets" feature, which would solve this directly, is only available
# since GNU Make 4.3.
$(srcdir)/zend_vm_execute.h $(srcdir)/zend_vm_opcodes.c: vm.gen.intermediate ;
.INTERMEDIATE: vm.gen.intermediate
vm.gen.intermediate: $(srcdir)/zend_vm_def.h $(srcdir)/zend_vm_execute.skl $(srcdir)/zend_vm_gen.php
	@if test ! -z "$(PHP)"; then \
		$(PHP) $(srcdir)/zend_vm_gen.php; \
	fi;

$(builddir)/zend_highlight.lo $(builddir)/zend_compile.lo: $(srcdir)/zend_language_parser.h

Zend/zend_execute.lo: $(srcdir)/zend_vm_execute.h $(srcdir)/zend_vm_opcodes.h
