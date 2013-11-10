The interactive PHP debugger
============================

phpdbg is an interactive debugger for PHP.

Implemented as a SAPI module, phpdbg can excerpt complete control over the environment without impacting the functionality or performance of your code.

phpdbg aims to be a lightweight, easy to use debugging platform for PHP5.5+

Features
========

 - step through debugging
 - break at function entry or file:line
 - showing debug backtraces
 - easy manipulation of the environment with built-in eval()
 - sapi agnositicism, easy integration
 
Planned
=======

 - mocking server environments/requests
 - userland script-ability
 - improve everything
 
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

Command Line Options
====================

The following switches are implemented (just like cli SAPI):

 - -n ignore php ini
 - -c search for php ini in path
 - -z load zend extension
 - -d define php ini entry

Screeny
=======

<img src="https://raw.github.com/krakjoe/phpdbg/master/phpdbg.png" alt="screenshot"/>
