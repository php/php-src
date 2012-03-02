/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

/* {{{ PHP_MBREGEX_FUNCTION_ENTRIES */
#define PHP_MBREGEX_FUNCTION_ENTRIES \
	PHP_FE(mb_regex_encoding,	arginfo_mb_regex_encoding) \
	PHP_FE(mb_regex_set_options,	arginfo_mb_regex_set_options) \
	PHP_FE(mb_ereg,			arginfo_mb_ereg) \
	PHP_FE(mb_eregi,			arginfo_mb_eregi) \
	PHP_FE(mb_ereg_replace,			arginfo_mb_ereg_replace) \
	PHP_FE(mb_eregi_replace,			arginfo_mb_eregi_replace) \
 	PHP_FE(mb_ereg_replace_callback,			arginfo_mb_ereg_replace_callback) \
	PHP_FE(mb_split,					arginfo_mb_split) \
	PHP_FE(mb_ereg_match,			arginfo_mb_ereg_match) \
	PHP_FE(mb_ereg_search,			arginfo_mb_ereg_search) \
	PHP_FE(mb_ereg_search_pos,		arginfo_mb_ereg_search_pos) \
	PHP_FE(mb_ereg_search_regs,		arginfo_mb_ereg_search_regs) \
	PHP_FE(mb_ereg_search_init,		arginfo_mb_ereg_search_init) \
	PHP_FE(mb_ereg_search_getregs,	arginfo_mb_ereg_search_getregs) \
	PHP_FE(mb_ereg_search_getpos,	arginfo_mb_ereg_search_getpos) \
	PHP_FE(mb_ereg_search_setpos,	arginfo_mb_ereg_search_setpos) \
	PHP_FALIAS(mbregex_encoding,	mb_regex_encoding,	arginfo_mb_regex_encoding) \
	PHP_FALIAS(mbereg,	mb_ereg,	arginfo_mb_ereg) \
	PHP_FALIAS(mberegi,	mb_eregi,	arginfo_mb_eregi) \
	PHP_FALIAS(mbereg_replace,	mb_ereg_replace,	arginfo_mb_ereg_replace) \
	PHP_FALIAS(mberegi_replace,	mb_eregi_replace,	arginfo_mb_eregi_replace) \
	PHP_FALIAS(mbsplit,	mb_split,	arginfo_mb_split) \
	PHP_FALIAS(mbereg_match,	mb_ereg_match,	arginfo_mb_ereg_match) \
	PHP_FALIAS(mbereg_search,	mb_ereg_search,	arginfo_mb_ereg_search) \
	PHP_FALIAS(mbereg_search_pos,	mb_ereg_search_pos,	arginfo_mb_ereg_search_pos) \
	PHP_FALIAS(mbereg_search_regs,	mb_ereg_search_regs,	arginfo_mb_ereg_search_regs) \
	PHP_FALIAS(mbereg_search_init,	mb_ereg_search_init,	arginfo_mb_ereg_search_init) \
	PHP_FALIAS(mbereg_search_getregs,	mb_ereg_search_getregs,	arginfo_mb_ereg_search_getregs) \
	PHP_FALIAS(mbereg_search_getpos,	mb_ereg_search_getpos,	arginfo_mb_ereg_search_getpos) \
	PHP_FALIAS(mbereg_search_setpos,	mb_ereg_search_setpos,	arginfo_mb_ereg_search_setpos)
/* }}} */

#define PHP_MBREGEX_MAXCACHE 50

PHP_MINIT_FUNCTION(mb_regex);
PHP_MSHUTDOWN_FUNCTION(mb_regex);
PHP_RINIT_FUNCTION(mb_regex);
PHP_RSHUTDOWN_FUNCTION(mb_regex);
PHP_MINFO_FUNCTION(mb_regex);

typedef struct _zend_mb_regex_globals zend_mb_regex_globals;

zend_mb_regex_globals *php_mb_regex_globals_alloc(TSRMLS_D);
void php_mb_regex_globals_free(zend_mb_regex_globals *pglobals TSRMLS_DC);
int php_mb_regex_set_mbctype(const char *enc TSRMLS_DC);
int php_mb_regex_set_default_mbctype(const char *encname TSRMLS_DC);
const char *php_mb_regex_get_mbctype(TSRMLS_D);
const char *php_mb_regex_get_default_mbctype(TSRMLS_D);

PHP_FUNCTION(mb_regex_encoding);
PHP_FUNCTION(mb_ereg);
PHP_FUNCTION(mb_eregi);
PHP_FUNCTION(mb_ereg_replace);
PHP_FUNCTION(mb_eregi_replace);
PHP_FUNCTION(mb_ereg_replace_callback);
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

