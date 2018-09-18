/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_LCG_H
#define PHP_LCG_H

#include "ext/standard/basic_functions.h"

typedef struct {
	int32_t s1;
	int32_t s2;
	int seeded;
} php_lcg_globals;

PHPAPI double php_combined_lcg(void);
PHP_FUNCTION(lcg_value);

PHP_MINIT_FUNCTION(lcg);

#ifdef ZTS
#define LCG(v) ZEND_TSRMG(lcg_globals_id, php_lcg_globals *, v)
#else
#define LCG(v) (lcg_globals.v)
#endif

#endif
