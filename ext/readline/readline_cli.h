/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   |         Johannes Schlueter <johannes@php.net>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "zend_smart_str_public.h"

ZEND_BEGIN_MODULE_GLOBALS(cli_readline)
	char *pager;
	char *prompt;
	smart_str *prompt_str;
ZEND_END_MODULE_GLOBALS(cli_readline)

#ifdef ZTS
# define CLIR_G(v) TSRMG(cli_readline_globals_id, zend_cli_readline_globals *, v)
#else
# define CLIR_G(v) (cli_readline_globals.v)
#endif

extern PHP_MINIT_FUNCTION(cli_readline);
extern PHP_MSHUTDOWN_FUNCTION(cli_readline);
extern PHP_MINFO_FUNCTION(cli_readline);

ZEND_EXTERN_MODULE_GLOBALS(cli_readline)


