/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_INI_H
#define PHP_INI_H

#include "zend_ini.h"

int php_init_config();
int php_shutdown_config(void);
void php_ini_delayed_modules_startup(TSRMLS_D);
zval *cfg_get_entry(char *name, uint name_length);

#define PHP_INI_USER	ZEND_INI_USER
#define PHP_INI_PERDIR	ZEND_INI_PERDIR
#define PHP_INI_SYSTEM	ZEND_INI_SYSTEM

#define PHP_INI_ALL 	ZEND_INI_ALL

#define php_ini_entry	zend_ini_entry

#define PHP_INI_MH		ZEND_INI_MH
#define PHP_INI_DISP	ZEND_INI_DISP

#define PHP_INI_BEGIN		ZEND_INI_BEGIN
#define PHP_INI_END			ZEND_INI_END

#define PHP_INI_ENTRY3_EX	ZEND_INI_ENTRY3_EX
#define PHP_INI_ENTRY3		ZEND_INI_ENTRY3
#define PHP_INI_ENTRY2_EX	ZEND_INI_ENTRY2_EX
#define PHP_INI_ENTRY2		ZEND_INI_ENTRY2
#define PHP_INI_ENTRY1_EX	ZEND_INI_ENTRY1_EX
#define PHP_INI_ENTRY1		ZEND_INI_ENTRY1
#define PHP_INI_ENTRY_EX	ZEND_INI_ENTRY_EX
#define PHP_INI_ENTRY		ZEND_INI_ENTRY

#define STD_PHP_INI_ENTRY		STD_ZEND_INI_ENTRY
#define STD_PHP_INI_ENTRY_EX	STD_ZEND_INI_ENTRY_EX
#define STD_PHP_INI_BOOLEAN		STD_ZEND_INI_BOOLEAN

#define PHP_INI_DISPLAY_ORIG	ZEND_INI_DISPLAY_ORIG
#define PHP_INI_DISPLAY_ACTIVE	ZEND_INI_DISPLAY_ACTIVE

#define PHP_INI_STAGE_STARTUP		ZEND_INI_STAGE_STARTUP
#define PHP_INI_STAGE_SHUTDOWN		ZEND_INI_STAGE_SHUTDOWN
#define PHP_INI_STAGE_ACTIVATE		ZEND_INI_STAGE_ACTIVATE
#define PHP_INI_STAGE_DEACTIVATE	ZEND_INI_STAGE_DEACTIVATE
#define PHP_INI_STAGE_RUNTIME		ZEND_INI_STAGE_RUNTIME

#define php_ini_boolean_displayer_cb	zend_ini_boolean_displayer_cb
#define php_ini_color_displayer_cb		zend_ini_color_displayer_cb

#define php_alter_ini_entry		zend_alter_ini_entry

#define php_ini_long	zend_ini_long
#define php_ini_double	zend_ini_double
#define php_ini_string	zend_ini_string

#endif /* PHP_INI_H */
