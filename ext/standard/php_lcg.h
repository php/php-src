/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_LCG_H
#define PHP_LCG_H

#include "ext/standard/basic_functions.h"

typedef struct {
	php_int32 s1;
	php_int32 s2;
	int seeded;
} php_lcg_globals;

PHPAPI double php_combined_lcg(TSRMLS_D);
PHP_FUNCTION(lcg_value);

PHP_MINIT_FUNCTION(lcg);
PHP_RINIT_FUNCTION(lcg);

#ifdef ZTS
#define LCG(v) TSRMG(lcg_globals_id, php_lcg_globals *, v)
#else
#define LCG(v) (lcg_globals.v)
#endif

#endif
