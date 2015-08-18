
#
# Zend
#

$(builddir)/zend_language_scanner.lo: $(srcdir)/zend_language_parser.h
$(builddir)/zend_ini_scanner.lo: $(srcdir)/zend_ini_parser.h

$(srcdir)/zend_language_scanner.c: $(srcdir)/zend_language_scanner.l
	@(cd $(top_srcdir); $(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt Zend/zend_language_scanner_defs.h -oZend/zend_language_scanner.c Zend/zend_language_scanner.l)

$(srcdir)/zend_language_parser.h: $(srcdir)/zend_language_parser.c
$(srcdir)/zend_language_parser.c: $(srcdir)/zend_language_parser.y
	@$(YACC) -p zend -v -d $(srcdir)/zend_language_parser.y -o $@

$(srcdir)/zend_ini_parser.h: $(srcdir)/zend_ini_parser.c
$(srcdir)/zend_ini_parser.c: $(srcdir)/zend_ini_parser.y
	@$(YACC) -p ini_ -v -d $(srcdir)/zend_ini_parser.y -o $@

$(srcdir)/zend_ini_scanner.c: $(srcdir)/zend_ini_scanner.l
	@(cd $(top_srcdir); $(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt Zend/zend_ini_scanner_defs.h -oZend/zend_ini_scanner.c Zend/zend_ini_scanner.l)

$(builddir)/zend_highlight.lo $(builddir)/zend_compile.lo: $(srcdir)/zend_language_parser.h
