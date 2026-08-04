/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   |         Johannes Schlueter <johannes@php.net>                        |
   +----------------------------------------------------------------------+
*/

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

char **php_readline_completion_cb(const char *text, int start, int end);

ZEND_EXTERN_MODULE_GLOBALS(cli_readline)
