
$(builddir)/minilua: $(srcdir)/jit/dynasm/minilua.c
	$(CC) $(srcdir)/jit/dynasm/minilua.c -lm -o $@

$(builddir)/jit/zend_jit_x86.c: $(srcdir)/jit/zend_jit_x86.dasc $(srcdir)/jit/dynasm/*.lua $(builddir)/minilua
	$(builddir)/minilua $(srcdir)/jit/dynasm/dynasm.lua  $(DASM_FLAGS) -o $@ $(srcdir)/jit/zend_jit_x86.dasc

$(builddir)/jit/zend_jit.lo: \
	$(builddir)/jit/zend_jit_x86.c \
	$(srcdir)/jit/zend_jit_helpers.c \
	$(srcdir)/jit/zend_jit_disasm_x86.c \
	$(srcdir)/jit/zend_jit_gdb.c \
	$(srcdir)/jit/zend_jit_perf_dump.c \
	$(srcdir)/jit/zend_jit_oprofile.c \
	$(srcdir)/jit/zend_jit_vtune.c \
	$(srcdir)/jit/zend_jit_trace.c \
	$(srcdir)/jit/zend_elf.c

# For non-GNU make, jit/zend_jit.lo and ./jit/zend_jit.lo are considered distinct targets.
# Use this workaround to allow building from inside ext/opcache.
jit/zend_jit.lo: $(builddir)/jit/zend_jit.lo
