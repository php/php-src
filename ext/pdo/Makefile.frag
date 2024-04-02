$(srcdir)/pdo_sql_parser.c: $(srcdir)/pdo_sql_parser.re
	@(cd $(top_srcdir); \
	if test -f ./pdo_sql_parser.re; then \
		$(RE2C) $(RE2C_FLAGS) --no-generation-date -o pdo_sql_parser.c pdo_sql_parser.re; \
	else \
		$(RE2C) $(RE2C_FLAGS) --no-generation-date -o ext/pdo/pdo_sql_parser.c ext/pdo/pdo_sql_parser.re; \
	fi)
