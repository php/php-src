/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies USA, Inc. (https://www.zend.com)     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.zend.com/sites/zend/files/pdfs/zend-engine-license-2.01.txt |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_GDB
#define ZEND_GDB

ZEND_API bool zend_gdb_register_code(const void *object, size_t size);
ZEND_API void zend_gdb_unregister_all(void);
ZEND_API bool zend_gdb_present(void);

#endif
