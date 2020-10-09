Opcache JIT
===========

This is the implementation of Opcache's JIT (Just-In-Time compiler),
This converts the PHP Virtual Machine's opcodes into x64/x86 assembly,
on POSIX platforms and Windows.

It generates native code directly from PHP byte-code and information collected
by the SSA static analysis framework (a part of the opcache optimizer).
Code is usually generated separately for each PHP byte-code instruction. Only
a few combinations are considered together (e.g. compare + conditional jump).

See [the JIT RFC](https://wiki.php.net/rfc/jit) for more details.

DynAsm
------

This uses [DynAsm](https://luajit.org/dynasm.html) (developed for LuaJIT project)
for the generation of native code.  It's a very lightweight and advanced tool,
but does assume good, and very low-level development knowledge of target
assembler languages. In the past we tried LLVM, but its code generation speed
was almost 100 times slower, making it prohibitively expensive to use.

[The unofficial DynASM Documentation](https://corsix.github.io/dynasm-doc/tutorial.html)
has a tutorial, reference, and instruction listing.

`zend_jit_x86.dasc` gets automatically converted to `zend_jit_x86.c` by the bundled
`dynasm` during `make`.
