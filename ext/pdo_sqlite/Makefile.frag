$(srcdir)/sqlite_sql_parser.c: $(srcdir)/sqlite_sql_parser.re
	@(cd $(top_srcdir); \
	if test -f ./sqlite_sql_parser.re; then \
		$(RE2C) $(RE2C_FLAGS) -o sqlite_sql_parser.c sqlite_sql_parser.re; \
	else \
		$(RE2C) $(RE2C_FLAGS) -o ext/pdo_sqlite/sqlite_sql_parser.c ext/pdo_sqlite/sqlite_sql_parser.re; \
	fi)
