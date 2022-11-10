New JIT implementation
======================

This branch contains a new **experimental** JIT implementation based
on [IR - Lightweight JIT Compilation Framework](https://github.com/dstogov/ir).

Despite of the PHP 8.* JIT approach, that generates native code directly from
PHP byte-code, this implementation generates intermediate representation (IR) and
delegates all lower-level tasks to IR Framework.

Both IR and PHP JIT implementation are under development. Only 50% of PHP byte-code
instructions are covered. Windows support is missing yet. ZTS and CLANG builds are
not tested. Few *.phpt tests fail...

New IR based JIT is disabled by default. It may be enable during PHP build process,
running configured with **--enable-opcache-jit-ir**. Otherwise, PHP is going to be
built with old JIT implementation.

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
./configure --enable-opcache-jit-ir --prefix=`pwd`/install --with-config-file-path=`pwd`/install/etc
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
