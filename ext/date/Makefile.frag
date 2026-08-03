$(srcdir)/lib/parse_date.c: $(srcdir)/lib/parse_date.re
	@(cd $(top_srcdir); \
	if test -f ./php_date.h; then \
		$(RE2C) $(RE2C_FLAGS) -b -o lib/parse_date.c lib/parse_date.re; \
	else \
		$(RE2C) $(RE2C_FLAGS) -b -o ext/date/lib/parse_date.c ext/date/lib/parse_date.re; \
	fi)

$(srcdir)/lib/parse_iso_intervals.c: $(srcdir)/lib/parse_iso_intervals.re
	@(cd $(top_srcdir); \
	if test -f ./php_date.h; then \
		$(RE2C) $(RE2C_FLAGS) -b -o lib/parse_iso_intervals.c lib/parse_iso_intervals.re; \
	else \
		$(RE2C) $(RE2C_FLAGS) -b -o ext/date/lib/parse_iso_intervals.c ext/date/lib/parse_iso_intervals.re; \
	fi)
