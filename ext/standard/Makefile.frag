
$(srcdir)/parsedate.c: $(srcdir)/parsedate.y

$(srcdir)/var_unserializer.c: $(srcdir)/var_unserializer.re
	re2c -b $(srcdir)/var_unserializer.re > $@

$(srcdir)/url_scanner_ex.c: $(srcdir)/url_scanner_ex.re
	re2c -b $(srcdir)/url_scanner_ex.re > $@

$(srcdir)/info.c: $(builddir)/../../main/build-defs.h
