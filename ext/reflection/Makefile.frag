$(top_srcdir)/Zend/zend_language_parser.c:
$(top_srcdir)/Zend/zend_language_scanner.c:
$(top_srcdir)/ext/reflection/php_reflection.c: $(top_srcdir)/Zend/zend_language_parser.h
$(builddir)/reflection.lo: $(top_srcdir)/Zend/zend_language_parser.c $(top_srcdir)/Zend/zend_language_scanner.c
