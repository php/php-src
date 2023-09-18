New JIT implementation
======================

This branch provides a new JIT implementation based on [IR - Lightweight
JIT Compilation Framework](https://github.com/dstogov/ir).

As opposed to the PHP 8.* JIT approach that generates native code directly from
PHP byte-code, this implementation generates intermediate representation (IR)
and delegates all lower-level tasks to the IR Framework. IR for JIT is like an
AST for compiler.

Key benefits of the new JIT implementation:
- Usage of IR opens possibilities for better optimization and register
  allocation (the resulting native code is more efficient)
- PHP doesn't have to care about most low-level details (different CPUs,
  calling conventions, TLS details, etc)
- it's much easier to implement support for new targets (e.g. RISCV)
- IR framework is going to be developed separately from PHP and may accept
  contributions from other projects (new optimizations, improvements, bug fixes)

Disadvantages:
- JIT compilation becomes slower (this is almost invisible for tracing
  JIT, but function JIT compilation of Wordpress becomes 4 times slower)

The necessary part of the IR Framework is embedded into php-src. So, the PR
doesn't introduce new dependencies.

The new JIT implementation successfully passes all CI workflows, but it's still
not mature and may cause failures. To reduce risks, this patch doesn't remove
the old JIT implementation (that is the same as PHP-8.3 JIT). It's possible
to build PHP with the old JIT by configuring with **--disable-opcache-jit-ir**.
In the future the old implementation should be removed.
