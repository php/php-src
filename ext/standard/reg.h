/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/


/* $Id$ */

#ifndef REG_H
#define REG_H

char *php_reg_replace(const char *pattern, const char *replace, const char *string, int icase, int extended);

PHP_FUNCTION(ereg);
PHP_FUNCTION(eregi);
PHP_FUNCTION(eregi_replace);
PHP_FUNCTION(ereg_replace);
PHP_FUNCTION(split);
PHP_FUNCTION(spliti);
PHPAPI PHP_FUNCTION(sql_regcase);

typedef struct {
	HashTable ht_rc;
} php_reg_globals;

PHP_MINIT_FUNCTION(regex);
PHP_MSHUTDOWN_FUNCTION(regex);
PHP_MINFO_FUNCTION(regex);


#ifdef ZTS
#define REGLS_D php_reg_globals *reg_globals
#define REGLS_DC , REGLS_D
#define REGLS_C reg_globals
#define REGLS_CC , REGLS_C
#define REG(v) (reg_globals->v)
#define REGLS_FETCH() php_reg_globals *reg_globals = ts_resource(reg_globals_id)
#else
#define REGLS_D
#define REGLS_DC
#define REGLS_C
#define REGLS_CC
#define REG(v) (reg_globals.v)
#define REGLS_FETCH()
#endif

#endif /* REG_H */
