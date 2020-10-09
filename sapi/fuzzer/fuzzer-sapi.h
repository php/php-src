/*
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
   | Authors: Johannes Schlüter <johanes@php.net>                         |
   |          Stanislav Malyshev <stas@php.net>                           |
   +----------------------------------------------------------------------+
 */

int fuzzer_init_php(void);
int fuzzer_request_startup(void);
void fuzzer_request_shutdown(void);
void fuzzer_setup_dummy_frame(void);
void fuzzer_call_php_func(const char *func_name, int nargs, char **params);
void fuzzer_call_php_func_zval(const char *func_name, int nargs, zval *args);
int fuzzer_do_request_from_buffer(
		char *filename, const char *data, size_t data_len, zend_bool execute);
