
$(srcdir)/parsedate.c: $(srcdir)/parsedate.y
	$(YACC) $(srcdir)/parsedate.y -o $@

$(srcdir)/var_unserializer.c: $(srcdir)/var_unserializer.re
	$(RE2C) -b $(srcdir)/var_unserializer.re > $@

$(srcdir)/url_scanner_ex.c: $(srcdir)/url_scanner_ex.re
	$(RE2C) -b $(srcdir)/url_scanner_ex.re > $@

$(builddir)/info.lo: $(builddir)/../../main/build-defs.h

$(builddir)/basic_functions.lo: Zend/zend_language_parser.h
