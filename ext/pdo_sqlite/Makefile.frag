
$(builddir)/sqlite/tool/lemon: $(srcdir)/sqlite/tool/lemon.c
	$(CC) $(srcdir)/sqlite/tool/lemon.c -o $@
	cp $(srcdir)/sqlite/tool/lempar.c $(builddir)/sqlite/tool/

$(builddir)/sqlite/tool/mkkeywordhash: $(srcdir)/sqlite/tool/mkkeywordhash.c
	$(CC) $(srcdir)/sqlite/tool/mkkeywordhash.c -o $@

$(srcdir)/sqlite/src/parse.c: $(srcdir)/sqlite/src/parse.y $(builddir)/sqlite/tool/lemon
	$(builddir)/sqlite/tool/lemon $(srcdir)/sqlite/src/parse.y

$(srcdir)/sqlite/src/parse.h: $(srcdir)/sqlite/src/parse.c

$(srcdir)/sqlite/src/opcodes.h: $(srcdir)/sqlite/src/parse.h
	cat $(srcdir)/sqlite/src/parse.h $(srcdir)/sqlite/src/vdbe.c | $(AWK) -f $(srcdir)/sqlite/mkopcodeh.awk > $@

$(srcdir)/sqlite/src/opcodes.c: $(srcdir)/sqlite/src/opcodes.h
	sort -n +2 $(srcdir)/sqlite/src/opcodes.h | $(AWK) -f $(srcdir)/sqlite/mkopcodec.awk > $@

$(srcdir)/sqlite/src/keywordhash.h: $(builddir)/sqlite/tool/mkkeywordhash
	$(srcdir)/sqlite/tool/mkkeywordhash > $(srcdir)/sqlite/src/keywordhash.h
