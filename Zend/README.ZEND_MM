Zend Memory Manager
===================

General:
--------

The goal of the new memory manager (available since PHP 5.2) is to reduce memory
allocation overhead and speedup memory management.

Debugging:
----------

Normal:

    $ sapi/cli/php -r 'leak();'

Zend MM disabled:

    $ USE_ZEND_ALLOC=0 valgrind --leak-check=full sapi/cli/php -r 'leak();'

Shared extensions:
------------------

Since PHP 5.3.11 it is possible to prevent shared extensions from unloading so
that valgrind can correctly track the memory leaks in shared extensions. For
this there is the ZEND_DONT_UNLOAD_MODULES environment variable. If set, then
DL_UNLOAD() is skipped during the shutdown of shared extensions.
