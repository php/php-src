/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_WEBHELPER_H
#define PHPDBG_WEBHELPER_H

#include "phpdbg_webdata_transfer.h"

extern zend_module_entry phpdbg_webhelper_module_entry;
#define phpext_phpdbg_webhelper_ptr &phpdbg_webhelper_module_entry

#ifdef ZTS
# define PHPDBG_WG(v) ZEND_TSRMG(phpdbg_webhelper_globals_id, zend_phpdbg_webhelper_globals *, v)
#else
# define PHPDBG_WG(v) (phpdbg_webhelper_globals.v)
#endif

/* {{{ structs */
ZEND_BEGIN_MODULE_GLOBALS(phpdbg_webhelper)
	char *auth;
	char *path;
ZEND_END_MODULE_GLOBALS(phpdbg_webhelper) /* }}} */

#endif /* PHPDBG_WEBHELPER_H */
