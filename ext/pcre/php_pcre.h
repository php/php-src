/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_PCRE_H
#define PHP_PCRE_H

#if HAVE_PCRE || HAVE_BUNDLED_PCRE

#if HAVE_BUNDLED_PCRE
#include "pcrelib/pcre.h"
#else
#include "pcre.h"
#endif

#if HAVE_LOCALE_H
#include <locale.h>
#endif

PHPAPI zend_string *php_pcre_replace(zend_string *regex, zend_string *subject_str, char *subject, int subject_len, zval *replace_val, int is_callable_replace, int limit, int *replace_count);
PHPAPI pcre* pcre_get_compiled_regex(zend_string *regex, pcre_extra **extra, int *options);
PHPAPI pcre* pcre_get_compiled_regex_ex(zend_string *regex, pcre_extra **extra, int *preg_options, int *coptions);

extern zend_module_entry pcre_module_entry;
#define pcre_module_ptr &pcre_module_entry

typedef struct {
	pcre *re;
	pcre_extra *extra;
	int preg_options;
	int capture_count;
	int name_count;
#if HAVE_SETLOCALE
	zend_string *locale;
	unsigned const char *tables;
#endif
	int compile_options;
	int refcount;
} pcre_cache_entry;

PHPAPI pcre_cache_entry* pcre_get_compiled_regex_cache(zend_string *regex);

PHPAPI void  php_pcre_match_impl(  pcre_cache_entry *pce, char *subject, int subject_len, zval *return_value,
	zval *subpats, int global, int use_flags, zend_long flags, zend_long start_offset);

PHPAPI zend_string *php_pcre_replace_impl(pcre_cache_entry *pce, zend_string *subject_str, char *subject, int subject_len, zval *return_value,
	int is_callable_replace, int limit, int *replace_count);

PHPAPI void  php_pcre_split_impl(  pcre_cache_entry *pce, char *subject, int subject_len, zval *return_value,
	zend_long limit_val, zend_long flags);

PHPAPI void  php_pcre_grep_impl(   pcre_cache_entry *pce, zval *input, zval *return_value,
	zend_long flags);

ZEND_BEGIN_MODULE_GLOBALS(pcre)
	HashTable pcre_cache;
	zend_long backtrack_limit;
	zend_long recursion_limit;
#ifdef PCRE_STUDY_JIT_COMPILE
	zend_bool jit;
#endif
	int  error_code;
ZEND_END_MODULE_GLOBALS(pcre)

#ifdef ZTS
# define PCRE_G(v) ZEND_TSRMG(pcre_globals_id, zend_pcre_globals *, v)
#else
# define PCRE_G(v)	(pcre_globals.v)
#endif

#else

#define pcre_module_ptr NULL

#endif /* HAVE_PCRE || HAVE_BUNDLED_PCRE */

#define phpext_pcre_ptr pcre_module_ptr

#endif /* PHP_PCRE_H */
