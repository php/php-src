/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Andrey Zmievski <andrey@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP_PCRE_H
#define _PHP_PCRE_H

#if HAVE_PCRE || HAVE_BUNDLED_PCRE

#if HAVE_BUNDLED_PCRE
#include "pcrelib/pcre.h"
#else
#include "pcre.h"
#endif

#if HAVE_LOCALE_H
#include <locale.h>
#endif

PHP_FUNCTION(preg_match);
PHP_FUNCTION(preg_match_all);
PHP_FUNCTION(preg_replace);
PHP_FUNCTION(preg_split);
PHP_FUNCTION(preg_quote);
PHP_FUNCTION(preg_grep);

extern zend_module_entry pcre_module_entry;
#define pcre_module_ptr &pcre_module_entry

typedef struct {
        pcre *re;
        pcre_extra *extra;
	int preg_options;
#if HAVE_SETLOCALE
	char *locale;
	unsigned const char *tables;
#endif
} pcre_cache_entry;

typedef struct {
	HashTable pcre_cache;
} php_pcre_globals;

#ifdef ZTS
# define PCRE_LS_D 	php_pcre_globals *pcre_globals
# define PCRE_LS_DC , PCRE_LS_D
# define PCRE_LS_C	pcre_globals
# define PCRE_LS_CC	, PCRE_LS_C
# define PCRE_G(v) 	(pcre_globals->v)
# define PCRE_LS_FETCH() php_pcre_globals *pcre_globals = ts_resource(pcre_globals_id);
#else
# define PCRE_LS_D
# define PCRE_LS_DC
# define PCRE_LS_C
# define PCRE_LS_CC
# define PCRE_G(v)	(pcre_globals.v)
# define PCRE_LS_FETCH()
extern ZEND_API php_pcre_globals pcre_globals;
#endif

#else

#define pcre_module_ptr NULL

#endif /* HAVE_PCRE || HAVE_BUNDLED_PCRE */

#define phpext_pcre_ptr pcre_module_ptr

#endif /* _PHP_PCRE_H */
