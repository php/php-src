/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
 
#ifndef _PHP_MBREGEX_H
#define _PHP_MBREGEX_H

#if HAVE_MBREGEX

#include "php.h"
#include "zend.h"
#include "mbregex.h"

/* {{{ PHP_MBREGEX_GLOBALS */
#define PHP_MBREGEX_GLOBALS \
	int default_mbctype; \
	int current_mbctype; \
	HashTable ht_rc; \
	zval **search_str; \
	zval *search_str_val; \
	unsigned int search_pos; \
	mb_regex_t *search_re; \
	struct mbre_registers *search_regs; \
	int regex_default_options;
/* }}} */

#define PHP_MBREGEX_MAXCACHE 50

int php_mb_regex_name2mbctype(const char *pname);
int php_mb_regex_set_options(int options TSRMLS_DC);
int php_mb_regex_set_options_by_string(const char *optstr, int len TSRMLS_DC);

PHP_FUNCTION(mb_regex_encoding);
PHP_FUNCTION(mb_ereg);
PHP_FUNCTION(mb_eregi);
PHP_FUNCTION(mb_ereg_replace);
PHP_FUNCTION(mb_eregi_replace);
PHP_FUNCTION(mb_split);
PHP_FUNCTION(mb_ereg_match);
PHP_FUNCTION(mb_ereg_search);
PHP_FUNCTION(mb_ereg_search_pos);
PHP_FUNCTION(mb_ereg_search_regs);
PHP_FUNCTION(mb_ereg_search_init);
PHP_FUNCTION(mb_ereg_search_getregs);
PHP_FUNCTION(mb_ereg_search_getpos);
PHP_FUNCTION(mb_ereg_search_setpos);
PHP_FUNCTION(mb_regex_set_options);

#endif /* HAVE_MBREGEX */

#endif /* _PHP_MBREGEX_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

