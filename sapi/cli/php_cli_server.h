/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_CLI_SERVER_H
#define PHP_CLI_SERVER_H

#include "SAPI.h"

extern const zend_function_entry server_additional_functions[];
extern sapi_module_struct cli_server_sapi_module;
extern int do_cli_server(int argc, char **argv);

ZEND_BEGIN_MODULE_GLOBALS(cli_server)
	short color;
ZEND_END_MODULE_GLOBALS(cli_server)

#ifdef ZTS
#define CLI_SERVER_G(v) ZEND_TSRMG(cli_server_globals_id, zend_cli_server_globals *, v)
ZEND_TSRMLS_CACHE_EXTERN()
#else
#define CLI_SERVER_G(v) (cli_server_globals.v)
#endif

#endif /* PHP_CLI_SERVER_H */
