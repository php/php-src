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
   | Author: David Eriksson <david@2good.com>                            |
   +----------------------------------------------------------------------+
 */

/*
 * $Id$
 * vim: syntax=c tabstop=2 shiftwidth=2
 */

#ifndef _PHP_ORBIT_H
#define _PHP_ORBIT_H

PHP_MINIT_FUNCTION(satellite);
PHP_MSHUTDOWN_FUNCTION(satellite);
extern zend_module_entry satellite_module_entry;

#define satellite_module_ptr &satellite_module_entry
#define phpext_satellite_ptr satellite_module_ptr

PHP_FUNCTION(satellite_load_idl);
PHP_FUNCTION(satellite_get_repository_id);
PHP_FUNCTION(satellite_object_to_string);

/* exception handling */
PHP_FUNCTION(satellite_caught_exception);
PHP_FUNCTION(satellite_exception_id);
PHP_FUNCTION(satellite_exception_value);

#endif  /* _PHP_ORBIT_H */
