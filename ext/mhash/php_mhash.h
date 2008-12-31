/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Nikos Mavroyanopoulos <nmav@hellug.gr> (HMAC, KEYGEN)       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef PHP_MHASH_H
#define PHP_MHASH_H

#if HAVE_LIBMHASH

#if PHP_API_VERSION < 19990421
#define  zend_module_entry zend_module_entry
#include "zend_modules.h"
#include "internal_functions.h"
#endif

extern zend_module_entry mhash_module_entry;
#define mhash_module_ptr &mhash_module_entry

PHP_MINIT_FUNCTION(mhash);
PHP_MINFO_FUNCTION(mhash);
PHP_FUNCTION(mhash_get_block_size);
PHP_FUNCTION(mhash_get_hash_name);
PHP_FUNCTION(mhash_count);
PHP_FUNCTION(mhash_keygen_s2k);
PHP_FUNCTION(mhash);

#else
#define mhash_module_ptr NULL
#endif

#define phpext_mhash_ptr mhash_module_ptr

#endif
