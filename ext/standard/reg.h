/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _REG_H
#define _REG_H

extern php3_module_entry regexp_module_entry;
#define regexp_module_ptr &regexp_module_entry

char *_php3_regreplace(const char *pattern, const char *replace, const char *string, int icase, int extended);

PHP_FUNCTION(ereg);
PHP_FUNCTION(eregi);
PHP_FUNCTION(eregi_replace);
PHP_FUNCTION(ereg_replace);
PHP_FUNCTION(split);
PHPAPI PHP_FUNCTION(sql_regcase);

typedef struct {
	HashTable ht_rc;
} php_reg_globals;


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

#define phpext_regex_ptr regexp_module_ptr

#endif /* _REG_H */
