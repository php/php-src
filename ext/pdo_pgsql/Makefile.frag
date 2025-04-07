$(srcdir)/pgsql_sql_parser.c: $(srcdir)/pgsql_sql_parser.re
	@(cd $(top_srcdir); \
	if test -f ./pgsql_sql_parser.re; then \
		$(RE2C) $(RE2C_FLAGS) -o pgsql_sql_parser.c pgsql_sql_parser.re; \
	else \
		$(RE2C) $(RE2C_FLAGS) -o ext/pdo_pgsql/pgsql_sql_parser.c ext/pdo_pgsql/pgsql_sql_parser.re; \
	fi)
