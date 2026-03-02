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
   | Authors: Johannes Schlüter <johanes@php.net>                         |
   |          Stanislav Malyshev <stas@php.net>                           |
   +----------------------------------------------------------------------+
 */

int fuzzer_init_php(const char *extra_ini);
int fuzzer_request_startup(void);
void fuzzer_request_shutdown(void);
void fuzzer_setup_dummy_frame(void);
void fuzzer_call_php_func(const char *func_name, int nargs, char **params);
void fuzzer_call_php_func_zval(const char *func_name, int nargs, zval *args);
int fuzzer_do_request_from_buffer(
		char *filename, const char *data, size_t data_len, bool execute,
		void (*before_shutdown)(void));
