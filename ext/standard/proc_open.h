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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

#ifdef PHP_WIN32
typedef HANDLE php_file_descriptor_t;
typedef DWORD php_process_id_t;
# define PHP_INVALID_FD INVALID_HANDLE_VALUE
#else
typedef int php_file_descriptor_t;
typedef pid_t php_process_id_t;
# define PHP_INVALID_FD (-1)
#endif

/* Environment block under Win32 is a NUL terminated sequence of NUL terminated
 *   name=value strings.
 * Under Unix, it is an argv style array. */
typedef struct _php_process_env {
	char *envp;
#ifndef PHP_WIN32
	char **envarray;
#endif
} php_process_env;

typedef struct _php_process_handle {
	php_process_id_t	child;
#ifdef PHP_WIN32
	HANDLE childHandle;
#endif
	int npipes;
	zend_resource **pipes;
	zend_string *command;
	php_process_env env;
} php_process_handle;
