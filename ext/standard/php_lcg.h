/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_LCG_H
#define PHP_LCG_H

typedef struct {
	long s1;
	long s2;
} php_lcg_globals;

double php_combined_lcg(void);
PHP_FUNCTION(lcg_value);
PHP_MINIT_FUNCTION(lcg);

#ifdef ZTS
#define LCGLS_D php_lcg_globals *lcg_globals
#define LCG(v) (lcg_globals->v)
#define LCGLS_FETCH() php_lcg_globals *lcg_globals = ts_resource(lcg_globals_id)
#else
#define LCGLS_D void
#define LCG(v) (lcg_globals.v)
#define LCGLS_FETCH()
#endif

#endif
