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
   | Authors: Jani Lehtimäki <jkl@njet.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_VAR_H
#define PHP_VAR_H

PHP_FUNCTION(var_dump);
PHP_FUNCTION(serialize);
PHP_FUNCTION(unserialize);

void php_var_dump(pval **struc, int level);
void php_var_serialize(pval *buf, pval **struc);
int php_var_unserialize(pval **rval, const char **p, const char *max);

PHPAPI zend_class_entry *php_create_empty_class(char *class_name,int len);

#endif /* PHP_VAR_H */
