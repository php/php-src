The interactive PHP debugger
============================

Implemented as a SAPI module, phpdbg can excerpt complete control over the environment without impacting the functionality or performance of your code.

phpdbg aims to be a lightweight, powerful, easy to use debugging platform for PHP5.5+

Features
========

 - Stepthrough Debugging
 - Flexible Breakpoints (Class Method, Function, File:Line, Address)
 - Easy Access to PHP with built-in eval()
 - Easy Access to Currently Executing Code
 - Userland API
 - SAPI Agnostic - Easily Integrated
 - PHP Configuration File Support
 - JIT Super Globals - Set Your Own !!
 - Optional readline Support - Comfortable Terminal Operation
 - Easy Operation - See Help :)

Planned
=======

 - Improve Everything :)
 
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

*Note: php must be configured with the switch --with-readline for phpdbg to support history, autocompletion, tab-listing etc*

Command Line Options
====================

The following switchess are implemented (just like cli SAPI):

 - -n ignore php ini
 - -c search for php ini in path
 - -z load zend extension
 - -d define php ini entry
 
The following switches change the default behaviour of phpdbg:

 - -v disables quietness
 - -s enabled stepping
 - -e sets execution context
 - -b boring - disables use of colour on the console

Screeny
=======

<img src="https://raw.github.com/krakjoe/phpdbg/master/phpdbg.png" alt="screenshot"/>
