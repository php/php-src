
$(srcdir)/parsedate.c: $(srcdir)/parsedate.y

$(srcdir)/var_unserializer.c: $(srcdir)/var_unserializer.re
	$(RE2C) -b $(srcdir)/var_unserializer.re > $@

$(srcdir)/url_scanner_ex.c: $(srcdir)/url_scanner_ex.re
	$(RE2C) -b $(srcdir)/url_scanner_ex.re > $@

$(builddir)/info.lo: $(builddir)/../../main/build-defs.h
