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
   | Authors: Jani Lehtimäki <jkl@njet.net>                               |
   +----------------------------------------------------------------------+
 */


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
