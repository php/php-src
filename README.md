phpdbg - The interactive PHP debugger
=====================================

phpdbg is an interactive debugger for PHP.

Implemented as a SAPI module, phpdbg can excerpt complete control over the environment without impacting the functionality or performance of your code.

Features
========

 - step through debugging
 - setting break points
 - showing debug backtraces
 - access to eval during execution
 - sapi agnositicism, easy integration
 
Installation
============

To install **phpdbg**, you must compile the source against your PHP installation sources, and enable the SAPI with the configure command.

```
cd /usr/src/php-src/sapi
git clone https://github.com/krakjoe/phpdbg
cd ../
./buildconf --force
./config.nice
make -j8
make install-phpdbg
```

Screeny
=======

<img src="https://raw.github.com/krakjoe/phpdbg/master/phpdbg.png" alt="screenshot"/>
