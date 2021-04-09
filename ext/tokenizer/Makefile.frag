$(top_srcdir)/Zend/zend_language_parser.c:
$(top_srcdir)/Zend/zend_language_scanner.c:
$(top_srcdir)/ext/tokenizer/tokenizer_data.c: $(top_srcdir)/Zend/zend_language_parser.y $(top_srcdir)/Zend/zend_language_parser.h
	@if test ! -z "$(PHP)"; then \
		$(PHP) $(srcdir)/tokenizer_data_gen.php; \
	fi;
$(builddir)/tokenizer.lo: $(top_srcdir)/Zend/zend_language_parser.c $(top_srcdir)/Zend/zend_language_scanner.c
