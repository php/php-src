
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

$(srcdir)/sess_sqlite.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/parse.c:	$(srcdir)/libsqlite/src/parse.y
	$(LEMON) $(srcdir)/libsqlite/src/parse.y

$(srcdir)/libsqlite/src/attach.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/auth.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/btree.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/btree_rb.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/build.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/copy.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/date.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/delete.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/encode.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/expr.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/func.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/hash.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/insert.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/main.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/os.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/pager.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/pragma.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/printf.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/random.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/select.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/table.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/tokenize.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/trigger.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/update.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/util.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/vacuum.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/vdbe.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/vdbeaux.c: $(srcdir)/libsqlite/src/sqlite.h

$(srcdir)/libsqlite/src/where.c: $(srcdir)/libsqlite/src/sqlite.h

