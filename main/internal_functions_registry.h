/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _INTERNAL_FUNCTIONS_REGISTRY_H
#define _INTERNAL_FUNCTIONS_REGISTRY_H

extern int php_init_mime(INIT_FUNC_ARGS);

#if APACHE
extern zend_module_entry apache_module_entry;
#define phpext_apache_ptr &apache_module_entry
extern void php_virtual(INTERNAL_FUNCTION_PARAMETERS);
#else
#define phpext_apache_ptr NULL
#endif

/* environment functions */
extern int php_init_environment(void);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
