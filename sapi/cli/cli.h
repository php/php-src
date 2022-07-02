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
  | Author: Johannes Schlueter <johannes@php.net>                        |
  +----------------------------------------------------------------------+
*/

#ifndef CLI_H
#define CLI_H

#ifdef PHP_WIN32
#   define PHP_CLI_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define PHP_CLI_API __attribute__ ((visibility("default")))
#else
#   define PHP_CLI_API
#endif


extern PHP_CLI_API ssize_t sapi_cli_single_write(const char *str, size_t str_length);

typedef struct  {
	size_t (*cli_shell_write)(const char *str, size_t str_length);
	size_t (*cli_shell_ub_write)(const char *str, size_t str_length);
	int (*cli_shell_run)(void);
} cli_shell_callbacks_t;

extern PHP_CLI_API cli_shell_callbacks_t *php_cli_get_shell_callbacks(void);

#endif /* CLI_H */
