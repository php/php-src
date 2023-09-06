New JIT implementation
======================

This branch provides a new JIT implementation based on [IR - Lightweight
JIT Compilation Framework](https://github.com/dstogov/ir).

Despite of the PHP 8.* JIT approach, that generates native code directly from
PHP byte-code, this implementation generates intermediate representation (IR)
and delegates all lower-level tasks to the IR Framework. IR for JIT is like an
AST for compiler.

Key benefits of the new JIT implementation:
- Usage of IR opens possibilities for better optimization and register
  allocation (the resulting native code is more efficient)
- PHP doesn't have to care about most low-level details (different CPUs,
  calling conventions, TLS details, etc)
- it's mach easier to implement support for new targets (e.g. RISCV)
- IR framework is going to be developed separately from PHP and may accept
  contibutions from other projects (new optimizations, improvemets, bug fixes)

Disadvantages:
- JIT compilation becomes slower (this is almost invisible for tracing
  JIT, but function JIT compilation of Wordpress becomes 4 times slower)

IR Framework is included into PHP as a GIT submodule. It should be fetched by

``git submodule update --init --recursive``

The new JIT implementation sucessfully passes all CI workflows, but it's still
not mature and may cause failures. To reduce risks, this patch doesn't remove
the old JIT implementation (that is the same as PHP-8.3 JIT). It's possible
to build PHP with the old JIT by configuring with **--disable-opcache-jit-ir**.
In the future the old implementation should be removed.

Building and Testing
====================

Install pre-requested libraries. PHP and their extensions may require different libraries.
JIT itself needs just **libcapstone** to produce disassembler output.

```
sudo dbf install capstone-devel
```

Build PHP

```
git clone -b php-ir --single-branch git@github.com:dstogov/php-src.git php-ir
cd php-ir
git submodule update --init --recursive
./buildconf --force
mkdir install
./configure --with-capstone --prefix=`pwd`/install --with-config-file-path=`pwd`/install/etc
make
make install
mkdir install/etc
cat > install/etc/php.ini <<EOL
zend_extension=opcache.so
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.jit_buffer_size=32M
opcache.jit=tracing
opcache.huge_code_pages=1
EOL
```

Check if opcache s loaded

```
sapi/cli/php -v | grep -i opcache
```

See JIT in action

```
sapi/cli/php -d opcache.jit=tracing -d opcache.jit_debug=1 Zend/bench.php
sapi/cli/php -d opcache.jit=function -d opcache.jit_debug=1 Zend/bench.php
```
