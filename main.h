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


#ifndef _MAIN_H
#define _MAIN_H

#include "zend_globals.h"
#include "php_globals.h"
#include "SAPI.h"

PHPAPI int php_request_startup(CLS_D ELS_DC PLS_DC SLS_DC);
PHPAPI void php_request_shutdown(void *dummy);
PHPAPI void php_request_shutdown_for_exec(void *dummy);
PHPAPI int php_module_startup(sapi_module_struct *sf);
PHPAPI void php_module_shutdown();
PHPAPI void php_module_shutdown_for_exec();
PHPAPI int php_module_shutdown_wrapper(sapi_module_struct *sapi_globals);

PHPAPI void php_execute_script(zend_file_handle *primary_file CLS_DC ELS_DC PLS_DC);

extern void php3_call_shutdown_functions(void);


/* configuration module */
extern int php3_init_config(void);
extern int php3_shutdown_config(void);

/* environment module */
extern int php3_init_environ(void);
extern int php3_shutdown_environ(void);

#endif
