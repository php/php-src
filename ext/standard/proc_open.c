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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

#if 0 && (defined(__linux__) || defined(sun) || defined(__IRIX__))
# define _BSD_SOURCE 		/* linux wants this when XOPEN mode is on */
# define _BSD_COMPAT		/* irix: uint32_t */
# define _XOPEN_SOURCE 500  /* turn on Unix98 */
# define __EXTENSIONS__	1	/* Solaris: uint32_t */
#endif

#include "php.h"
#include <stdio.h>
#include <ctype.h>
#include "php_string.h"
#include "ext/standard/head.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"
#include "exec.h"
#include "php_globals.h"
#include "SAPI.h"
#include "main/php_network.h"
#include "zend_smart_string.h"

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#include <signal.h>

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

/* This symbol is defined in ext/standard/config.m4.
 * Essentially, it is set if you HAVE_FORK || PHP_WIN32
 * Other platforms may modify that configure check and add suitable #ifdefs
 * around the alternate code.
 * */
#ifdef PHP_CAN_SUPPORT_PROC_OPEN

#if 0 && HAVE_PTSNAME && HAVE_GRANTPT && HAVE_UNLOCKPT && HAVE_SYS_IOCTL_H && HAVE_TERMIOS_H
# include <sys/ioctl.h>
# include <termios.h>
# define PHP_CAN_DO_PTS	1
#endif

#include "proc_open.h"

static int le_proc_open;

/* {{{ _php_array_to_envp */
static php_process_env_t _php_array_to_envp(zval *environment, int is_persistent)
{
	zval *element;
	php_process_env_t env;
	zend_string *key, *str;
#ifndef PHP_WIN32
	char **ep;
#endif
	char *p;
	size_t cnt, sizeenv = 0;
	HashTable *env_hash;

	memset(&env, 0, sizeof(env));

	if (!environment) {
		return env;
	}

	cnt = zend_hash_num_elements(Z_ARRVAL_P(environment));

	if (cnt < 1) {
#ifndef PHP_WIN32
		env.envarray = (char **) pecalloc(1, sizeof(char *), is_persistent);
#endif
		env.envp = (char *) pecalloc(4, 1, is_persistent);
		return env;
	}

	ALLOC_HASHTABLE(env_hash);
	zend_hash_init(env_hash, cnt, NULL, NULL, 0);

	/* first, we have to get the size of all the elements in the hash */
	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(environment), key, element) {
		str = zval_get_string(element);

		if (ZSTR_LEN(str) == 0) {
			zend_string_release_ex(str, 0);
			continue;
		}

		sizeenv += ZSTR_LEN(str) + 1;

		if (key && ZSTR_LEN(key)) {
			sizeenv += ZSTR_LEN(key) + 1;
			zend_hash_add_ptr(env_hash, key, str);
		} else {
			zend_hash_next_index_insert_ptr(env_hash, str);
		}
	} ZEND_HASH_FOREACH_END();

#ifndef PHP_WIN32
	ep = env.envarray = (char **) pecalloc(cnt + 1, sizeof(char *), is_persistent);
#endif
	p = env.envp = (char *) pecalloc(sizeenv + 4, 1, is_persistent);

	ZEND_HASH_FOREACH_STR_KEY_PTR(env_hash, key, str) {
#ifndef PHP_WIN32
		*ep = p;
		++ep;
#endif

		if (key) {
			memcpy(p, ZSTR_VAL(key), ZSTR_LEN(key));
			p += ZSTR_LEN(key);
			*p++ = '=';
		}

		memcpy(p, ZSTR_VAL(str), ZSTR_LEN(str));
		p += ZSTR_LEN(str);
		*p++ = '\0';
		zend_string_release_ex(str, 0);
	} ZEND_HASH_FOREACH_END();

	assert((uint32_t)(p - env.envp) <= sizeenv);

	zend_hash_destroy(env_hash);
	FREE_HASHTABLE(env_hash);

	return env;
}
/* }}} */

/* {{{ _php_free_envp */
static void _php_free_envp(php_process_env_t env, int is_persistent)
{
#ifndef PHP_WIN32
	if (env.envarray) {
		pefree(env.envarray, is_persistent);
	}
#endif
	if (env.envp) {
		pefree(env.envp, is_persistent);
	}
}
/* }}} */

/* {{{ proc_open_rsrc_dtor */
static void proc_open_rsrc_dtor(zend_resource *rsrc)
{
	struct php_process_handle *proc = (struct php_process_handle*)rsrc->ptr;
	int i;
#ifdef PHP_WIN32
	DWORD wstatus;
#elif HAVE_SYS_WAIT_H
	int wstatus;
	int waitpid_options = 0;
	pid_t wait_pid;
#endif

	/* Close all handles to avoid a deadlock */
	for (i = 0; i < proc->npipes; i++) {
		if (proc->pipes[i] != 0) {
			GC_DELREF(proc->pipes[i]);
			zend_list_close(proc->pipes[i]);
			proc->pipes[i] = 0;
		}
	}

#ifdef PHP_WIN32
	if (FG(pclose_wait)) {
		WaitForSingleObject(proc->childHandle, INFINITE);
	}
	GetExitCodeProcess(proc->childHandle, &wstatus);
	if (wstatus == STILL_ACTIVE) {
		FG(pclose_ret) = -1;
	} else {
		FG(pclose_ret) = wstatus;
	}
	CloseHandle(proc->childHandle);

#elif HAVE_SYS_WAIT_H

	if (!FG(pclose_wait)) {
		waitpid_options = WNOHANG;
	}
	do {
		wait_pid = waitpid(proc->child, &wstatus, waitpid_options);
	} while (wait_pid == -1 && errno == EINTR);

	if (wait_pid <= 0) {
		FG(pclose_ret) = -1;
	} else {
		if (WIFEXITED(wstatus))
			wstatus = WEXITSTATUS(wstatus);
		FG(pclose_ret) = wstatus;
	}

#else
	FG(pclose_ret) = -1;
#endif
	_php_free_envp(proc->env, proc->is_persistent);
	pefree(proc->pipes, proc->is_persistent);
	pefree(proc->command, proc->is_persistent);
	pefree(proc, proc->is_persistent);

}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(proc_open) */
PHP_MINIT_FUNCTION(proc_open)
{
	le_proc_open = zend_register_list_destructors_ex(proc_open_rsrc_dtor, NULL, "process", module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ proto bool proc_terminate(resource process [, int signal])
   kill a process opened by proc_open */
PHP_FUNCTION(proc_terminate)
{
	zval *zproc;
	struct php_process_handle *proc;
	zend_long sig_no = SIGTERM;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_RESOURCE(zproc)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sig_no)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if ((proc = (struct php_process_handle *)zend_fetch_resource(Z_RES_P(zproc), "process", le_proc_open)) == NULL) {
		RETURN_FALSE;
	}

#ifdef PHP_WIN32
	if (TerminateProcess(proc->childHandle, 255)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
#else
	if (kill(proc->child, sig_no) == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
#endif
}
/* }}} */

/* {{{ proto int proc_close(resource process)
   close a process opened by proc_open */
PHP_FUNCTION(proc_close)
{
	zval *zproc;
	struct php_process_handle *proc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(zproc)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if ((proc = (struct php_process_handle *)zend_fetch_resource(Z_RES_P(zproc), "process", le_proc_open)) == NULL) {
		RETURN_FALSE;
	}

	FG(pclose_wait) = 1;
	zend_list_close(Z_RES_P(zproc));
	FG(pclose_wait) = 0;
	RETURN_LONG(FG(pclose_ret));
}
/* }}} */

/* {{{ proto array proc_get_status(resource process)
   get information about a process opened by proc_open */
PHP_FUNCTION(proc_get_status)
{
	zval *zproc;
	struct php_process_handle *proc;
#ifdef PHP_WIN32
	DWORD wstatus;
#elif HAVE_SYS_WAIT_H
	int wstatus;
	pid_t wait_pid;
#endif
	int running = 1, signaled = 0, stopped = 0;
	int exitcode = -1, termsig = 0, stopsig = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(zproc)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if ((proc = (struct php_process_handle *)zend_fetch_resource(Z_RES_P(zproc), "process", le_proc_open)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	add_assoc_string(return_value, "command", proc->command);
	add_assoc_long(return_value, "pid", (zend_long) proc->child);

#ifdef PHP_WIN32

	GetExitCodeProcess(proc->childHandle, &wstatus);

	running = wstatus == STILL_ACTIVE;
	exitcode = running ? -1 : wstatus;

#elif HAVE_SYS_WAIT_H

	errno = 0;
	wait_pid = waitpid(proc->child, &wstatus, WNOHANG|WUNTRACED);

	if (wait_pid == proc->child) {
		if (WIFEXITED(wstatus)) {
			running = 0;
			exitcode = WEXITSTATUS(wstatus);
		}
		if (WIFSIGNALED(wstatus)) {
			running = 0;
			signaled = 1;

			termsig = WTERMSIG(wstatus);
		}
		if (WIFSTOPPED(wstatus)) {
			stopped = 1;
			stopsig = WSTOPSIG(wstatus);
		}
	} else if (wait_pid == -1) {
		running = 0;
	}
#endif

	add_assoc_bool(return_value, "running", running);
	add_assoc_bool(return_value, "signaled", signaled);
	add_assoc_bool(return_value, "stopped", stopped);
	add_assoc_long(return_value, "exitcode", exitcode);
	add_assoc_long(return_value, "termsig", termsig);
	add_assoc_long(return_value, "stopsig", stopsig);
}
/* }}} */

/* {{{ handy definitions for portability/readability */
#ifdef PHP_WIN32
# define pipe(pair)		(CreatePipe(&pair[0], &pair[1], &security, 0) ? 0 : -1)

# define COMSPEC_NT	"cmd.exe"

static inline HANDLE dup_handle(HANDLE src, BOOL inherit, BOOL closeorig)
{
	HANDLE copy, self = GetCurrentProcess();

	if (!DuplicateHandle(self, src, self, &copy, 0, inherit, DUPLICATE_SAME_ACCESS |
				(closeorig ? DUPLICATE_CLOSE_SOURCE : 0)))
		return NULL;
	return copy;
}

static inline HANDLE dup_fd_as_handle(int fd)
{
	return dup_handle((HANDLE)_get_osfhandle(fd), TRUE, FALSE);
}

# define close_descriptor(fd)	CloseHandle(fd)
#else
# define close_descriptor(fd)	close(fd)
#endif

#define DESC_PIPE		1
#define DESC_FILE		2
#define DESC_REDIRECT	3
#define DESC_PARENT_MODE_WRITE	8

struct php_proc_open_descriptor_item {
	int index; 							/* desired fd number in child process */
	php_file_descriptor_t parentend, childend;	/* fds for pipes in parent/child */
	int mode;							/* mode for proc_open code */
	int mode_flags;						/* mode flags for opening fds */
};
/* }}} */

static zend_string *get_valid_arg_string(zval *zv, int elem_num) {
	zend_string *str = zval_get_string(zv);
	if (!str) {
		return NULL;
	}

	if (strlen(ZSTR_VAL(str)) != ZSTR_LEN(str)) {
		php_error_docref(NULL, E_WARNING,
			"Command array element %d contains a null byte", elem_num);
		zend_string_release(str);
		return NULL;
	}

	return str;
}

#ifdef PHP_WIN32
static void append_backslashes(smart_string *str, size_t num_bs) {
	size_t i;
	for (i = 0; i < num_bs; i++) {
		smart_string_appendc(str, '\\');
	}
}

/* See https://docs.microsoft.com/en-us/cpp/cpp/parsing-cpp-command-line-arguments */
static void append_win_escaped_arg(smart_string *str, char *arg) {
	char c;
	size_t num_bs = 0;
	smart_string_appendc(str, '"');
	while ((c = *arg)) {
		if (c == '\\') {
			num_bs++;
		} else {
			if (c == '"') {
				/* Backslashes before " need to be doubled. */
				num_bs = num_bs * 2 + 1;
			}
			append_backslashes(str, num_bs);
			smart_string_appendc(str, c);
			num_bs = 0;
		}
		arg++;
	}
	append_backslashes(str, num_bs * 2);
	smart_string_appendc(str, '"');
}

static char *create_win_command_from_args(HashTable *args) {
	smart_string str = {0};
	zval *arg_zv;
	zend_bool is_prog_name = 1;
	int elem_num = 0;

	ZEND_HASH_FOREACH_VAL(args, arg_zv) {
		zend_string *arg_str = get_valid_arg_string(arg_zv, ++elem_num);
		if (!arg_str) {
			smart_string_free(&str);
			return NULL;
		}

		if (!is_prog_name) {
			smart_string_appendc(&str, ' ');
		}

		append_win_escaped_arg(&str, ZSTR_VAL(arg_str));

		is_prog_name = 0;
		zend_string_release(arg_str);
	} ZEND_HASH_FOREACH_END();
	smart_string_0(&str);
	return str.c;
}
#endif

/* {{{ proto resource proc_open(string|array command, array descriptorspec, array &pipes [, string cwd [, array env [, array other_options]]])
   Run a process with more control over it's file descriptors */
PHP_FUNCTION(proc_open)
{
	zval *command_zv;
	char *command = NULL, *cwd = NULL;
	size_t cwd_len = 0;
	zval *descriptorspec;
	zval *pipes;
	zval *environment = NULL;
	zval *other_options = NULL;
	php_process_env_t env;
	int ndesc = 0;
	int i;
	zval *descitem = NULL;
	zend_string *str_index;
	zend_ulong nindex;
	struct php_proc_open_descriptor_item *descriptors = NULL;
	int ndescriptors_array;
#ifdef PHP_WIN32
	PROCESS_INFORMATION pi;
	HANDLE childHandle;
	STARTUPINFOW si;
	BOOL newprocok;
	SECURITY_ATTRIBUTES security;
	DWORD dwCreateFlags = 0;
	UINT old_error_mode;
	char cur_cwd[MAXPATHLEN];
	wchar_t *cmdw = NULL, *cwdw = NULL, *envpw = NULL;
	size_t tmp_len;
	int suppress_errors = 0;
	int bypass_shell = 0;
	int blocking_pipes = 0;
	int create_process_group = 0;
	int create_new_console = 0;
#else
	char **argv = NULL;
#endif
	php_process_id_t child;
	struct php_process_handle *proc;
	int is_persistent = 0; /* TODO: ensure that persistent procs will work */
#if PHP_CAN_DO_PTS
	php_file_descriptor_t dev_ptmx = -1;	/* master */
	php_file_descriptor_t slave_pty = -1;
#endif

	ZEND_PARSE_PARAMETERS_START(3, 6)
		Z_PARAM_ZVAL(command_zv)
		Z_PARAM_ARRAY(descriptorspec)
		Z_PARAM_ZVAL(pipes)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_EX(cwd, cwd_len, 1, 0)
		Z_PARAM_ARRAY_EX(environment, 1, 0)
		Z_PARAM_ARRAY_EX(other_options, 1, 0)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	memset(&env, 0, sizeof(env));

	if (Z_TYPE_P(command_zv) == IS_ARRAY) {
		zval *arg_zv;
		uint32_t num_elems = zend_hash_num_elements(Z_ARRVAL_P(command_zv));
		if (num_elems == 0) {
			php_error_docref(NULL, E_WARNING, "Command array must have at least one element");
			RETURN_FALSE;
		}

#ifdef PHP_WIN32
		bypass_shell = 1;
		command = create_win_command_from_args(Z_ARRVAL_P(command_zv));
		if (!command) {
			RETURN_FALSE;
		}
#else
		argv = safe_emalloc(sizeof(char *), num_elems + 1, 0);
		i = 0;
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(command_zv), arg_zv) {
			zend_string *arg_str = get_valid_arg_string(arg_zv, i + 1);
			if (!arg_str) {
				argv[i] = NULL;
				goto exit_fail;
			}

			if (i == 0) {
				command = pestrdup(ZSTR_VAL(arg_str), is_persistent);
			}

			argv[i++] = estrdup(ZSTR_VAL(arg_str));
			zend_string_release(arg_str);
		} ZEND_HASH_FOREACH_END();
		argv[i] = NULL;

		/* As the array is non-empty, we should have found a command. */
		ZEND_ASSERT(command);
#endif
	} else {
		convert_to_string(command_zv);
		command = pestrdup(Z_STRVAL_P(command_zv), is_persistent);
	}

#ifdef PHP_WIN32
	if (other_options) {
		zval *item = zend_hash_str_find(Z_ARRVAL_P(other_options), "suppress_errors", sizeof("suppress_errors") - 1);
		if (item != NULL) {
			if (Z_TYPE_P(item) == IS_TRUE || ((Z_TYPE_P(item) == IS_LONG) && Z_LVAL_P(item))) {
				suppress_errors = 1;
			}
		}

		/* TODO Deprecate in favor of array command? */
		item = zend_hash_str_find(Z_ARRVAL_P(other_options), "bypass_shell", sizeof("bypass_shell") - 1);
		if (item != NULL) {
			if (Z_TYPE_P(item) == IS_TRUE || ((Z_TYPE_P(item) == IS_LONG) && Z_LVAL_P(item))) {
				bypass_shell = 1;
			}
		}

		item = zend_hash_str_find(Z_ARRVAL_P(other_options), "blocking_pipes", sizeof("blocking_pipes") - 1);
		if (item != NULL) {
			if (Z_TYPE_P(item) == IS_TRUE || ((Z_TYPE_P(item) == IS_LONG) && Z_LVAL_P(item))) {
				blocking_pipes = 1;
			}
		}

		item = zend_hash_str_find(Z_ARRVAL_P(other_options), "create_process_group", sizeof("create_process_group") - 1);
		if (item != NULL) {
			if (Z_TYPE_P(item) == IS_TRUE || ((Z_TYPE_P(item) == IS_LONG) && Z_LVAL_P(item))) {
				create_process_group = 1;
			}
		}

		item = zend_hash_str_find(Z_ARRVAL_P(other_options), "create_new_console", sizeof("create_new_console") - 1);
		if (item != NULL) {
			if (Z_TYPE_P(item) == IS_TRUE || ((Z_TYPE_P(item) == IS_LONG) && Z_LVAL_P(item))) {
				create_new_console = 1;
			}
		}
	}
#endif

	if (environment) {
		env = _php_array_to_envp(environment, is_persistent);
	}

	ndescriptors_array = zend_hash_num_elements(Z_ARRVAL_P(descriptorspec));

	descriptors = safe_emalloc(sizeof(struct php_proc_open_descriptor_item), ndescriptors_array, 0);

	memset(descriptors, 0, sizeof(struct php_proc_open_descriptor_item) * ndescriptors_array);

#ifdef PHP_WIN32
	/* we use this to allow the child to inherit handles */
	memset(&security, 0, sizeof(security));
	security.nLength = sizeof(security);
	security.bInheritHandle = TRUE;
	security.lpSecurityDescriptor = NULL;
#endif

	/* walk the descriptor spec and set up files/pipes */
	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(descriptorspec), nindex, str_index, descitem) {
		zval *ztype;

		if (str_index) {
			php_error_docref(NULL, E_WARNING, "descriptor spec must be an integer indexed array");
			goto exit_fail;
		}

		descriptors[ndesc].index = (int)nindex;

		if (Z_TYPE_P(descitem) == IS_RESOURCE) {
			/* should be a stream - try and dup the descriptor */
			php_stream *stream;
			php_socket_t fd;

			php_stream_from_zval(stream, descitem);

			if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&fd, REPORT_ERRORS)) {
				goto exit_fail;
			}

#ifdef PHP_WIN32
			descriptors[ndesc].childend = dup_fd_as_handle((int)fd);
			if (descriptors[ndesc].childend == NULL) {
				php_error_docref(NULL, E_WARNING, "unable to dup File-Handle for descriptor %d", nindex);
				goto exit_fail;
			}
#else
			descriptors[ndesc].childend = dup(fd);
			if (descriptors[ndesc].childend < 0) {
				php_error_docref(NULL, E_WARNING, "unable to dup File-Handle for descriptor " ZEND_ULONG_FMT " - %s", nindex, strerror(errno));
				goto exit_fail;
			}
#endif
			descriptors[ndesc].mode = DESC_FILE;

		} else if (Z_TYPE_P(descitem) != IS_ARRAY) {
			php_error_docref(NULL, E_WARNING, "Descriptor item must be either an array or a File-Handle");
			goto exit_fail;
		} else {

			if ((ztype = zend_hash_index_find(Z_ARRVAL_P(descitem), 0)) != NULL) {
				if (!try_convert_to_string(ztype)) {
					goto exit_fail;
				}
			} else {
				php_error_docref(NULL, E_WARNING, "Missing handle qualifier in array");
				goto exit_fail;
			}

			if (strcmp(Z_STRVAL_P(ztype), "pipe") == 0) {
				php_file_descriptor_t newpipe[2];
				zval *zmode;

				if ((zmode = zend_hash_index_find(Z_ARRVAL_P(descitem), 1)) != NULL) {
					if (!try_convert_to_string(zmode)) {
						goto exit_fail;
					}
				} else {
					php_error_docref(NULL, E_WARNING, "Missing mode parameter for 'pipe'");
					goto exit_fail;
				}

				descriptors[ndesc].mode = DESC_PIPE;

				if (0 != pipe(newpipe)) {
					php_error_docref(NULL, E_WARNING, "unable to create pipe %s", strerror(errno));
					goto exit_fail;
				}

				if (strncmp(Z_STRVAL_P(zmode), "w", 1) != 0) {
					descriptors[ndesc].parentend = newpipe[1];
					descriptors[ndesc].childend = newpipe[0];
					descriptors[ndesc].mode |= DESC_PARENT_MODE_WRITE;
				} else {
					descriptors[ndesc].parentend = newpipe[0];
					descriptors[ndesc].childend = newpipe[1];
				}
#ifdef PHP_WIN32
				/* don't let the child inherit the parent side of the pipe */
				descriptors[ndesc].parentend = dup_handle(descriptors[ndesc].parentend, FALSE, TRUE);
#endif
				descriptors[ndesc].mode_flags = descriptors[ndesc].mode & DESC_PARENT_MODE_WRITE ? O_WRONLY : O_RDONLY;
#ifdef PHP_WIN32
				if (Z_STRLEN_P(zmode) >= 2 && Z_STRVAL_P(zmode)[1] == 'b')
					descriptors[ndesc].mode_flags |= O_BINARY;
#endif

			} else if (strcmp(Z_STRVAL_P(ztype), "file") == 0) {
				zval *zfile, *zmode;
				php_socket_t fd;
				php_stream *stream;

				descriptors[ndesc].mode = DESC_FILE;

				if ((zfile = zend_hash_index_find(Z_ARRVAL_P(descitem), 1)) != NULL) {
					if (!try_convert_to_string(zfile)) {
						goto exit_fail;
					}
				} else {
					php_error_docref(NULL, E_WARNING, "Missing file name parameter for 'file'");
					goto exit_fail;
				}

				if ((zmode = zend_hash_index_find(Z_ARRVAL_P(descitem), 2)) != NULL) {
					if (!try_convert_to_string(zmode)) {
						goto exit_fail;
					}
				} else {
					php_error_docref(NULL, E_WARNING, "Missing mode parameter for 'file'");
					goto exit_fail;
				}

				/* try a wrapper */
				stream = php_stream_open_wrapper(Z_STRVAL_P(zfile), Z_STRVAL_P(zmode),
						REPORT_ERRORS|STREAM_WILL_CAST, NULL);

				/* force into an fd */
				if (stream == NULL || FAILURE == php_stream_cast(stream,
							PHP_STREAM_CAST_RELEASE|PHP_STREAM_AS_FD,
							(void **)&fd, REPORT_ERRORS)) {
					goto exit_fail;
				}

#ifdef PHP_WIN32
				descriptors[ndesc].childend = dup_fd_as_handle((int)fd);
				_close((int)fd);

				/* simulate the append mode by fseeking to the end of the file
				this introduces a potential race-condition, but it is the best we can do, though */
				if (strchr(Z_STRVAL_P(zmode), 'a')) {
					SetFilePointer(descriptors[ndesc].childend, 0, NULL, FILE_END);
				}
#else
				descriptors[ndesc].childend = fd;
#endif
			} else if (strcmp(Z_STRVAL_P(ztype), "redirect") == 0) {
				zval *ztarget = zend_hash_index_find_deref(Z_ARRVAL_P(descitem), 1);
				struct php_proc_open_descriptor_item *target = NULL;
				php_file_descriptor_t childend;

				if (!ztarget) {
					php_error_docref(NULL, E_WARNING, "Missing redirection target");
					goto exit_fail;
				}
				if (Z_TYPE_P(ztarget) != IS_LONG) {
					php_error_docref(NULL, E_WARNING, "Redirection target must be an integer");
					goto exit_fail;
				}

				for (i = 0; i < ndesc; i++) {
					if (descriptors[i].index == Z_LVAL_P(ztarget)) {
						target = &descriptors[i];
						break;
					}
				}
				if (target) {
					childend = target->childend;
				} else {
					if (Z_LVAL_P(ztarget) < 0 || Z_LVAL_P(ztarget) > 2) {
						php_error_docref(NULL, E_WARNING,
							"Redirection target " ZEND_LONG_FMT " not found", Z_LVAL_P(ztarget));
						goto exit_fail;
					}

					/* Support referring to a stdin/stdout/stderr pipe adopted from the parent,
					 * which happens whenever an explicit override is not provided. */
#ifndef PHP_WIN32
					childend = Z_LVAL_P(ztarget);
#else
					switch (Z_LVAL_P(ztarget)) {
						case 0: childend = GetStdHandle(STD_INPUT_HANDLE); break;
						case 1: childend = GetStdHandle(STD_OUTPUT_HANDLE); break;
						case 2: childend = GetStdHandle(STD_ERROR_HANDLE); break;
						EMPTY_SWITCH_DEFAULT_CASE()
					}
#endif
				}

#ifdef PHP_WIN32
				descriptors[ndesc].childend = dup_handle(childend, TRUE, FALSE);
				if (descriptors[ndesc].childend == NULL) {
					php_error_docref(NULL, E_WARNING,
						"Failed to dup() for descriptor " ZEND_LONG_FMT, nindex);
					goto exit_fail;
				}
#else
				descriptors[ndesc].childend = dup(childend);
				if (descriptors[ndesc].childend < 0) {
					php_error_docref(NULL, E_WARNING,
						"Failed to dup() for descriptor " ZEND_LONG_FMT " - %s",
						nindex, strerror(errno));
					goto exit_fail;
				}
#endif
				descriptors[ndesc].mode = DESC_REDIRECT;
			} else if (strcmp(Z_STRVAL_P(ztype), "null") == 0) {
#ifndef PHP_WIN32
				descriptors[ndesc].childend = open("/dev/null", O_RDWR);
				if (descriptors[ndesc].childend < 0) {
					php_error_docref(NULL, E_WARNING,
						"Failed to open /dev/null - %s", strerror(errno));
					goto exit_fail;
				}
#else
				descriptors[ndesc].childend = CreateFileA(
					"nul", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, 0, NULL);
				if (descriptors[ndesc].childend == NULL) {
					php_error_docref(NULL, E_WARNING, "Failed to open nul");
					goto exit_fail;
				}
#endif
				descriptors[ndesc].mode = DESC_FILE;
			} else if (strcmp(Z_STRVAL_P(ztype), "pty") == 0) {
#if PHP_CAN_DO_PTS
				if (dev_ptmx == -1) {
					/* open things up */
					dev_ptmx = open("/dev/ptmx", O_RDWR);
					if (dev_ptmx == -1) {
						php_error_docref(NULL, E_WARNING, "failed to open /dev/ptmx, errno %d", errno);
						goto exit_fail;
					}
					grantpt(dev_ptmx);
					unlockpt(dev_ptmx);
					slave_pty = open(ptsname(dev_ptmx), O_RDWR);

					if (slave_pty == -1) {
						php_error_docref(NULL, E_WARNING, "failed to open slave pty, errno %d", errno);
						goto exit_fail;
					}
				}
				descriptors[ndesc].mode = DESC_PIPE;
				descriptors[ndesc].childend = dup(slave_pty);
				descriptors[ndesc].parentend = dup(dev_ptmx);
				descriptors[ndesc].mode_flags = O_RDWR;
#else
				php_error_docref(NULL, E_WARNING, "pty pseudo terminal not supported on this system");
				goto exit_fail;
#endif
			} else {
				php_error_docref(NULL, E_WARNING, "%s is not a valid descriptor spec/mode", Z_STRVAL_P(ztype));
				goto exit_fail;
			}
		}
		ndesc++;
	} ZEND_HASH_FOREACH_END();

#ifdef PHP_WIN32
	if (cwd == NULL) {
		char *getcwd_result;
		getcwd_result = VCWD_GETCWD(cur_cwd, MAXPATHLEN);
		if (!getcwd_result) {
			php_error_docref(NULL, E_WARNING, "Cannot get current directory");
			goto exit_fail;
		}
		cwd = cur_cwd;
	}
	cwdw = php_win32_cp_any_to_w(cwd);
	if (!cwdw) {
		php_error_docref(NULL, E_WARNING, "CWD conversion failed");
		goto exit_fail;
	}

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;

	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	/* redirect stdin/stdout/stderr if requested */
	for (i = 0; i < ndesc; i++) {
		switch(descriptors[i].index) {
			case 0:
				si.hStdInput = descriptors[i].childend;
				break;
			case 1:
				si.hStdOutput = descriptors[i].childend;
				break;
			case 2:
				si.hStdError = descriptors[i].childend;
				break;
		}
	}


	memset(&pi, 0, sizeof(pi));

	if (suppress_errors) {
		old_error_mode = SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
	}

	dwCreateFlags = NORMAL_PRIORITY_CLASS;
	if(strcmp(sapi_module.name, "cli") != 0) {
		dwCreateFlags |= CREATE_NO_WINDOW;
	}
	if (create_process_group) {
		dwCreateFlags |= CREATE_NEW_PROCESS_GROUP;
	}
	if (create_new_console) {
		dwCreateFlags |= CREATE_NEW_CONSOLE;
	}
	envpw = php_win32_cp_env_any_to_w(env.envp);
	if (envpw) {
		dwCreateFlags |= CREATE_UNICODE_ENVIRONMENT;
	} else  {
		if (env.envp) {
			php_error_docref(NULL, E_WARNING, "ENV conversion failed");
			goto exit_fail;
		}
	}

	cmdw = php_win32_cp_conv_any_to_w(command, strlen(command), &tmp_len);
	if (!cmdw) {
		php_error_docref(NULL, E_WARNING, "Command conversion failed");
		goto exit_fail;
	}

	if (bypass_shell) {
		newprocok = CreateProcessW(NULL, cmdw, &security, &security, TRUE, dwCreateFlags, envpw, cwdw, &si, &pi);
	} else {
		int ret;
		size_t len;
		wchar_t *cmdw2;


		len = (sizeof(COMSPEC_NT) + sizeof(" /c ") + tmp_len + 1);
		cmdw2 = (wchar_t *)malloc(len * sizeof(wchar_t));
		if (!cmdw2) {
			php_error_docref(NULL, E_WARNING, "Command conversion failed");
			goto exit_fail;
		}
		ret = _snwprintf(cmdw2, len, L"%hs /c %s", COMSPEC_NT, cmdw);

		if (-1 == ret) {
			free(cmdw2);
			php_error_docref(NULL, E_WARNING, "Command conversion failed");
			goto exit_fail;
		}

		newprocok = CreateProcessW(NULL, cmdw2, &security, &security, TRUE, dwCreateFlags, envpw, cwdw, &si, &pi);
		free(cmdw2);
	}

	free(cwdw);
	cwdw = NULL;
	free(cmdw);
	cmdw = NULL;
	free(envpw);
	envpw = NULL;

	if (suppress_errors) {
		SetErrorMode(old_error_mode);
	}

	if (FALSE == newprocok) {
		DWORD dw = GetLastError();

		/* clean up all the descriptors */
		for (i = 0; i < ndesc; i++) {
			CloseHandle(descriptors[i].childend);
			if (descriptors[i].parentend) {
				CloseHandle(descriptors[i].parentend);
			}
		}
		php_error_docref(NULL, E_WARNING, "CreateProcess failed, error code - %u", dw);
		goto exit_fail;
	}

	childHandle = pi.hProcess;
	child       = pi.dwProcessId;
	CloseHandle(pi.hThread);
#elif HAVE_FORK
	/* the unix way */
	child = fork();

	if (child == 0) {
		/* this is the child process */

#if PHP_CAN_DO_PTS
		if (dev_ptmx >= 0) {
			int my_pid = getpid();

#ifdef TIOCNOTTY
			/* detach from original tty. Might only need this if isatty(0) is true */
			ioctl(0,TIOCNOTTY,NULL);
#else
			setsid();
#endif
			/* become process group leader */
			setpgid(my_pid, my_pid);
			tcsetpgrp(0, my_pid);
		}
#endif

#if PHP_CAN_DO_PTS
		if (dev_ptmx >= 0) {
			close(dev_ptmx);
			close(slave_pty);
		}
#endif
		/* close those descriptors that we just opened for the parent stuff,
		 * dup new descriptors into required descriptors and close the original
		 * cruft */
		for (i = 0; i < ndesc; i++) {
			switch (descriptors[i].mode & ~DESC_PARENT_MODE_WRITE) {
				case DESC_PIPE:
					close(descriptors[i].parentend);
					break;
			}
			if (dup2(descriptors[i].childend, descriptors[i].index) < 0)
				perror("dup2");
			if (descriptors[i].childend != descriptors[i].index)
				close(descriptors[i].childend);
		}

		if (cwd) {
			php_ignore_value(chdir(cwd));
		}

		if (argv) {
			/* execvpe() is non-portable, use environ instead. */
			if (env.envarray) {
				environ = env.envarray;
			}
			execvp(command, argv);
		} else {
			if (env.envarray) {
				execle("/bin/sh", "sh", "-c", command, NULL, env.envarray);
			} else {
				execl("/bin/sh", "sh", "-c", command, NULL);
			}
		}
		_exit(127);

	} else if (child < 0) {
		/* failed to fork() */

		/* clean up all the descriptors */
		for (i = 0; i < ndesc; i++) {
			close(descriptors[i].childend);
			if (descriptors[i].parentend)
				close(descriptors[i].parentend);
		}

		php_error_docref(NULL, E_WARNING, "fork failed - %s", strerror(errno));

		goto exit_fail;

	}
#else
# error You lose (configure should not have let you get here)
#endif
	/* we forked/spawned and this is the parent */

	pipes = zend_try_array_init(pipes);
	if (!pipes) {
		goto exit_fail;
	}

	proc = (struct php_process_handle*)pemalloc(sizeof(struct php_process_handle), is_persistent);
	proc->is_persistent = is_persistent;
	proc->command = command;
	proc->pipes = pemalloc(sizeof(zend_resource *) * ndesc, is_persistent);
	proc->npipes = ndesc;
	proc->child = child;
#ifdef PHP_WIN32
	proc->childHandle = childHandle;
#endif
	proc->env = env;

#if PHP_CAN_DO_PTS
	if (dev_ptmx >= 0) {
		close(dev_ptmx);
		close(slave_pty);
	}
#endif

	/* clean up all the child ends and then open streams on the parent
	 * ends, where appropriate */
	for (i = 0; i < ndesc; i++) {
		char *mode_string=NULL;
		php_stream *stream = NULL;

		close_descriptor(descriptors[i].childend);

		switch (descriptors[i].mode & ~DESC_PARENT_MODE_WRITE) {
			case DESC_PIPE:
				switch(descriptors[i].mode_flags) {
#ifdef PHP_WIN32
					case O_WRONLY|O_BINARY:
						mode_string = "wb";
						break;
					case O_RDONLY|O_BINARY:
						mode_string = "rb";
						break;
#endif
					case O_WRONLY:
						mode_string = "w";
						break;
					case O_RDONLY:
						mode_string = "r";
						break;
					case O_RDWR:
						mode_string = "r+";
						break;
				}
#ifdef PHP_WIN32
				stream = php_stream_fopen_from_fd(_open_osfhandle((zend_intptr_t)descriptors[i].parentend,
							descriptors[i].mode_flags), mode_string, NULL);
				php_stream_set_option(stream, PHP_STREAM_OPTION_PIPE_BLOCKING, blocking_pipes, NULL);
#else
				stream = php_stream_fopen_from_fd(descriptors[i].parentend, mode_string, NULL);
# if defined(F_SETFD) && defined(FD_CLOEXEC)
				/* mark the descriptor close-on-exec, so that it won't be inherited by potential other children */
				fcntl(descriptors[i].parentend, F_SETFD, FD_CLOEXEC);
# endif
#endif
				if (stream) {
					zval retfp;

					/* nasty hack; don't copy it */
					stream->flags |= PHP_STREAM_FLAG_NO_SEEK;

					php_stream_to_zval(stream, &retfp);
					add_index_zval(pipes, descriptors[i].index, &retfp);

					proc->pipes[i] = Z_RES(retfp);
					Z_ADDREF(retfp);
				}
				break;
			default:
				proc->pipes[i] = NULL;
		}
	}

#ifndef PHP_WIN32
	if (argv) {
		char **arg = argv;
		while (*arg != NULL) {
			efree(*arg);
			arg++;
		}
		efree(argv);
	}
#endif

	efree(descriptors);
	ZVAL_RES(return_value, zend_register_resource(proc, le_proc_open));
	return;

exit_fail:
	if (descriptors) {
		efree(descriptors);
	}
	_php_free_envp(env, is_persistent);
	if (command) {
		pefree(command, is_persistent);
	}
#ifdef PHP_WIN32
	free(cwdw);
	free(cmdw);
	free(envpw);
#else
	if (argv) {
		char **arg = argv;
		while (*arg != NULL) {
			efree(*arg);
			arg++;
		}
		efree(argv);
	}
#endif
#if PHP_CAN_DO_PTS
	if (dev_ptmx >= 0) {
		close(dev_ptmx);
	}
	if (slave_pty >= 0) {
		close(slave_pty);
	}
#endif
	RETURN_FALSE;

}
/* }}} */

#endif /* PHP_CAN_SUPPORT_PROC_OPEN */
