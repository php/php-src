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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP_ARRAY_H
#define _PHP_ARRAY_H

extern zend_module_entry array_module_entry;
#define array_module_ptr &array_module_entry

PHP_MINIT_FUNCTION(array);
PHP_RINIT_FUNCTION(array);

PHP_FUNCTION(ksort);
PHP_FUNCTION(krsort);
PHP_FUNCTION(asort);
PHP_FUNCTION(arsort);
PHP_FUNCTION(sort);
PHP_FUNCTION(rsort);
PHP_FUNCTION(usort);
PHP_FUNCTION(uasort);
PHP_FUNCTION(uksort);
PHP_FUNCTION(array_walk);
PHP_FUNCTION(count);
PHP_FUNCTION(end);
PHP_FUNCTION(prev);
PHP_FUNCTION(next);
PHP_FUNCTION(reset);
PHP_FUNCTION(current);
PHP_FUNCTION(key);
PHP_FUNCTION(min);
PHP_FUNCTION(max);
PHP_FUNCTION(in_array);
PHP_FUNCTION(extract);
PHP_FUNCTION(compact);
PHP_FUNCTION(range);
PHP_FUNCTION(shuffle);
PHP_FUNCTION(multisort);
PHP_FUNCTION(array_push);
PHP_FUNCTION(array_pop);
PHP_FUNCTION(array_shift);
PHP_FUNCTION(array_unshift);
PHP_FUNCTION(array_splice);
PHP_FUNCTION(array_slice);
PHP_FUNCTION(array_merge);
PHP_FUNCTION(array_keys);
PHP_FUNCTION(array_values);
PHP_FUNCTION(array_count_values);
PHP_FUNCTION(array_reverse);
PHP_FUNCTION(array_pad);


#define phpext_array_ptr array_module_ptr
#endif /* _PHP_ARRAY_H */
