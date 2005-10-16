$(srcdir)/libsqlite/src/parse.c: $(srcdir)/libsqlite/src/parse.y
	@$(LEMON) $(srcdir)/libsqlite/src/parse.y 
