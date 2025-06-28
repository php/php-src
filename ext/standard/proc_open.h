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
#ifdef HAVE_SYS_WAIT_H
	/* We can only request the status once before it becomes unavailable.
	 * Cache the result so we can request it multiple times. */
	int cached_exit_wait_status_value;
	bool has_cached_exit_wait_status;
#endif
} php_process_handle;
