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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_MAIN_H
#define PHP_MAIN_H

#include "zend_globals.h"
#include "php_globals.h"
#include "SAPI.h"
#include "zend_modules.h"

BEGIN_EXTERN_C()

/* Returns the PHP version the engine was built with. This is useful for
 * extensions which want to know the version of PHP at run-time, rather than
 * the version they were built with at compile-time.
 */
PHPAPI const char *php_version(void);

/* Returns the PHP version id the engine was built with. This is useful for
 * extensions which want to know the version of PHP at run-time, rather than
 * the version they were built with at compile-time.
 */
PHPAPI unsigned int php_version_id(void);

PHPAPI zend_result php_request_startup(void);
PHPAPI void php_request_shutdown(void *dummy);
PHPAPI zend_result php_module_startup(sapi_module_struct *sf, zend_module_entry *additional_module);
PHPAPI void php_module_shutdown(void);
PHPAPI zend_result php_module_child_startup(sapi_module_struct *sf);
PHPAPI int php_module_shutdown_wrapper(sapi_module_struct *sapi_globals);

PHPAPI zend_result php_register_extensions(zend_module_entry * const * ptr, int count);

PHPAPI bool php_execute_script(zend_file_handle *primary_file);
PHPAPI bool php_execute_script_ex(zend_file_handle *primary_file, zval *retval);
PHPAPI int php_execute_simple_script(zend_file_handle *primary_file, zval *ret);
PHPAPI zend_result php_lint_script(zend_file_handle *file);

PHPAPI void php_handle_aborted_connection(void);
PHPAPI int php_handle_auth_data(const char *auth);

PHPAPI void php_html_puts(const char *str, size_t siz);
PHPAPI zend_result php_stream_open_for_zend_ex(zend_file_handle *handle, int mode);

/* environment module */
extern int php_init_environ(void);
extern int php_shutdown_environ(void);

#ifdef ZTS
PHPAPI void php_reserve_tsrm_memory(void);
PHPAPI bool php_tsrm_startup_ex(int expected_threads);
PHPAPI bool php_tsrm_startup(void);

#define PHP_ZTS 1
#else
#define PHP_ZTS 0
#endif

#ifdef PHP_WIN32
#define PHP_OS_STR "WINNT"
#else
#define PHP_OS_STR PHP_OS
#endif

END_EXTERN_C()

#endif
