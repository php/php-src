
# sqlite has some generated source files; this specifies how to
# build them

# We avoid building these last three by bundling the generated versions
# in our release packages

$(srcdir)/libsqlite/src/opcodes.c: $(srcdir)/libsqlite/src/opcodes.h $(srcdir)/libsqlite/src/sqlite.h
	echo '/* Automatically generated file.  Do not edit */' >$@
	echo 'char *sqliteOpcodeNames[] = { "???", ' >>$@
	grep '^case OP_' $(srcdir)/libsqlite/src/vdbe.c | \
	  sed -e 's/^.*OP_/  "/' -e 's/:.*$$/", /' >>$@
	echo '};' >>$@
	
$(srcdir)/libsqlite/src/opcodes.h: $(srcdir)/libsqlite/src/vdbe.c
	echo '/* Automatically generated file.  Do not edit */' >$@
	grep '^case OP_' $(srcdir)/libsqlite/src/vdbe.c | \
	  sed -e 's/://' | \
	  awk '{printf "#define %-30s %3d\n", $$2, ++cnt}' >>$@

$(srcdir)/libsqlite/src/sqlite.h: $(srcdir)/libsqlite/src/sqlite.h.in
	sed -e s/--VERS--/$(SQLITE_VERSION)/ -e s/--ENCODING--/$(SQLITE_ENCODING)/ $(srcdir)/libsqlite/src/sqlite.h.in >$(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/sqlite.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/parse.c:	$(srcdir)/libsqlite/src/parse.y
	$(LEMON) $(srcdir)/libsqlite/src/parse.y


