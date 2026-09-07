/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
 */

#ifndef _PHP_MBREGEX_H
#define _PHP_MBREGEX_H

#ifdef HAVE_MBREGEX

#include "php.h"
#include "zend.h"

#define PHP_MBREGEX_MAXCACHE 50

PHP_MINIT_FUNCTION(mb_regex);
PHP_MSHUTDOWN_FUNCTION(mb_regex);
PHP_RINIT_FUNCTION(mb_regex);
PHP_RSHUTDOWN_FUNCTION(mb_regex);
PHP_MINFO_FUNCTION(mb_regex);

extern char php_mb_oniguruma_version[256];

typedef struct _zend_mb_regex_globals zend_mb_regex_globals;

zend_mb_regex_globals *php_mb_regex_globals_alloc(void);
void php_mb_regex_globals_free(zend_mb_regex_globals *pglobals);
int php_mb_regex_set_mbctype(const char *enc);
int php_mb_regex_set_default_mbctype(const char *encname);
const char *php_mb_regex_get_mbctype(void);
const char *php_mb_regex_get_default_mbctype(void);

#endif /* HAVE_MBREGEX */

#endif /* _PHP_MBREGEX_H */
