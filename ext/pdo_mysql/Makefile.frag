$(srcdir)/mysql_sql_parser.c: $(srcdir)/mysql_sql_parser.re
	@(cd $(top_srcdir); \
	if test -f ./mysql_sql_parser.re; then \
		$(RE2C) $(RE2C_FLAGS) -o mysql_sql_parser.c mysql_sql_parser.re; \
	else \
		$(RE2C) $(RE2C_FLAGS) -o ext/pdo_mysql/mysql_sql_parser.c ext/pdo_mysql/mysql_sql_parser.re; \
	fi)
