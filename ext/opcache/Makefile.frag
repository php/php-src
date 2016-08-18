
$(builddir)/minilua: $(srcdir)/dynasm/minilua.c
	$(CC) $(srcdir)/dynasm/minilua.c -lm -o $@

$(builddir)/jit/zend_jit_x86.c: $(srcdir)/jit/zend_jit_x86.dasc $(srcdir)/dynasm/*.lua $(builddir)/minilua
	$(builddir)/minilua $(srcdir)/dynasm/dynasm.lua  $(DASM_FLAGS) -o $@ $(srcdir)/jit/zend_jit_x86.dasc

$(builddir)/jit/zend_jit.lo: $(builddir)/jit/zend_jit_x86.c

