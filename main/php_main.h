/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id$ */


#ifndef PHP_MAIN_H
#define PHP_MAIN_H

#include "zend_globals.h"
#include "php_globals.h"
#include "SAPI.h"

PHPAPI int php_request_startup(TSRMLS_D);
PHPAPI void php_request_shutdown(void *dummy);
PHPAPI void php_request_shutdown_for_exec(void *dummy);
PHPAPI int php_module_startup(sapi_module_struct *sf, zend_module_entry *additional_modules, uint num_additional_modules);
PHPAPI void php_module_shutdown(TSRMLS_D);
PHPAPI void php_module_shutdown_for_exec(void);
PHPAPI int php_module_shutdown_wrapper(sapi_module_struct *sapi_globals);

PHPAPI int php_startup_extensions(zend_module_entry **ptr, int count);

PHPAPI int php_execute_script(zend_file_handle *primary_file TSRMLS_DC);
PHPAPI int php_handle_special_queries(TSRMLS_D);
PHPAPI int php_lint_script(zend_file_handle *file TSRMLS_DC);

PHPAPI void php_handle_aborted_connection(void);
PHPAPI int php_handle_auth_data(const char *auth TSRMLS_DC);

PHPAPI void php_html_puts(const char *str, uint siz TSRMLS_DC);

extern void php_call_shutdown_functions(void);

/* environment module */
extern int php_init_environ(void);
extern int php_shutdown_environ(void);

#endif
