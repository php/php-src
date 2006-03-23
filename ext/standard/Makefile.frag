
$(srcdir)/parsedate.c: $(srcdir)/parsedate.y
	$(YACC) -v -d $(srcdir)/parsedate.y -o $(srcdir)/parsedate.c

$(srcdir)/var_unserializer.c: $(srcdir)/var_unserializer.re
	@(cd $(top_srcdir); $(RE2C) -b -o ext/standard/var_unserializer.c ext/standard/var_unserializer.re)

$(srcdir)/url_scanner_ex.c: $(srcdir)/url_scanner_ex.re
	@(cd $(top_srcdir); $(RE2C) -b -o ext/standard/url_scanner_ex.c	ext/standard/url_scanner_ex.re)

$(builddir)/info.lo: $(builddir)/../../main/build-defs.h
