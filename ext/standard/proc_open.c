/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#if 0 && (defined(__linux__) || defined(sun) || defined(__IRIX__))
# define _BSD_SOURCE 		/* linux wants this when XOPEN mode is on */
# define _BSD_COMPAT		/* irix: uint */
# define _XOPEN_SOURCE 500  /* turn on Unix98 */
# define __EXTENSIONS__	1	/* Solaris: uint */
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

#ifdef NETWARE
#include <proc.h>
#include <library.h>
#endif

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif

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
static php_process_env_t _php_array_to_envp(zval *environment, int is_persistent TSRMLS_DC)
{
	zval **element;
	php_process_env_t env;
	char *string_key, *data;
#ifndef PHP_WIN32
	char **ep;
#endif
	char *p;
	uint string_length, cnt, l, sizeenv=0, el_len;
	ulong num_key;
	HashTable *target_hash;
	HashPosition pos;

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

	target_hash = HASH_OF(environment);
	if (!target_hash) {
		return env;
	}

	/* first, we have to get the size of all the elements in the hash */
	for (zend_hash_internal_pointer_reset_ex(target_hash, &pos);
			zend_hash_get_current_data_ex(target_hash, (void **) &element, &pos) == SUCCESS;
			zend_hash_move_forward_ex(target_hash, &pos)) {

		if (Z_TYPE_PP(element) != IS_STRING) {
			zval tmp;

			MAKE_COPY_ZVAL(element, &tmp);
			convert_to_string(&tmp);
			el_len = Z_STRLEN(tmp);

			zval_dtor(&tmp);
		} else {
			el_len = Z_STRLEN_PP(element);
		}
		if (el_len == 0) {
			continue;
		}

		sizeenv += el_len+1;

		switch (zend_hash_get_current_key_ex(target_hash, &string_key, &string_length, &num_key, 0, &pos)) {
			case HASH_KEY_IS_STRING:
				if (string_length == 0) {
					continue;
				}
				sizeenv += string_length;
				break;
		}
	}

#ifndef PHP_WIN32
	ep = env.envarray = (char **) pecalloc(cnt + 1, sizeof(char *), is_persistent);
#endif
	p = env.envp = (char *) pecalloc(sizeenv + 4, 1, is_persistent);

	for (zend_hash_internal_pointer_reset_ex(target_hash, &pos);
			zend_hash_get_current_data_ex(target_hash, (void **) &element, &pos) == SUCCESS;
			zend_hash_move_forward_ex(target_hash, &pos)) {
		zval tmp;

		if (Z_TYPE_PP(element) != IS_STRING) {
			MAKE_COPY_ZVAL(element, &tmp);
			convert_to_string(&tmp);
		} else {
			tmp = **element;
		}

		el_len = Z_STRLEN(tmp);

		if (el_len == 0) {
			goto next_element;
		}

		data = Z_STRVAL(tmp);
		switch (zend_hash_get_current_key_ex(target_hash, &string_key, &string_length, &num_key, 0, &pos)) {
			case HASH_KEY_IS_STRING:
				if (string_length == 0) {
					goto next_element;
				}

				l = string_length + el_len + 1;
				memcpy(p, string_key, string_length);
				strncat(p, "=", 1);
				strncat(p, data, el_len);

#ifndef PHP_WIN32
				*ep = p;
				++ep;
#endif
				p += l;
				break;
			case HASH_KEY_IS_LONG:
				memcpy(p,data,el_len);
#ifndef PHP_WIN32
				*ep = p;
				++ep;
#endif
				p += el_len + 1;
				break;
			case HASH_KEY_NON_EXISTENT:
				break;
		}

next_element:
		if (Z_TYPE_PP(element) != IS_STRING) {
			zval_dtor(&tmp);
		}
	}

	assert((uint)(p - env.envp) <= sizeenv);

	zend_hash_internal_pointer_reset_ex(target_hash, &pos);

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
static void proc_open_rsrc_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
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
			zend_list_delete(proc->pipes[i]);
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

/* {{{ proto bool proc_terminate(resource process [, long signal])
   kill a process opened by proc_open */
PHP_FUNCTION(proc_terminate)
{
	zval *zproc;
	struct php_process_handle *proc;
	long sig_no = SIGTERM;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zproc, &sig_no) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(proc, struct php_process_handle *, &zproc, -1, "process", le_proc_open);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zproc) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(proc, struct php_process_handle *, &zproc, -1, "process", le_proc_open);

	FG(pclose_wait) = 1;
	zend_list_delete(Z_LVAL_P(zproc));
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zproc) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(proc, struct php_process_handle *, &zproc, -1, "process", le_proc_open);

	array_init(return_value);

	add_assoc_string(return_value, "command", proc->command, 1);
	add_assoc_long(return_value, "pid", (long) proc->child);

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
#ifdef NETWARE
			termsig = WIFTERMSIG(wstatus);
#else
			termsig = WTERMSIG(wstatus);
#endif
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
#define DESC_PARENT_MODE_WRITE	8

struct php_proc_open_descriptor_item {
	int index; 							/* desired fd number in child process */
	php_file_descriptor_t parentend, childend;	/* fds for pipes in parent/child */
	int mode;							/* mode for proc_open code */
	int mode_flags;						/* mode flags for opening fds */
};
/* }}} */

/* {{{ proto resource proc_open(string command, array descriptorspec, array &pipes [, string cwd [, array env [, array other_options]]])
   Run a process with more control over it's file descriptors */
PHP_FUNCTION(proc_open)
{
	char *command, *cwd=NULL;
	int command_len, cwd_len = 0;
	zval *descriptorspec;
	zval *pipes;
	zval *environment = NULL;
	zval *other_options = NULL;
	php_process_env_t env;
	int ndesc = 0;
	int i;
	zval **descitem = NULL;
	HashPosition pos;
	struct php_proc_open_descriptor_item descriptors[PHP_PROC_OPEN_MAX_DESCRIPTORS];
#ifdef PHP_WIN32
	PROCESS_INFORMATION pi;
	HANDLE childHandle;
	STARTUPINFO si;
	BOOL newprocok;
	SECURITY_ATTRIBUTES security;
	DWORD dwCreateFlags = 0;
	char *command_with_cmd;
	UINT old_error_mode;
	char cur_cwd[MAXPATHLEN];
#endif
#ifdef NETWARE
	char** child_argv = NULL;
	char* command_dup = NULL;
	char* orig_cwd = NULL;
	int command_num_args = 0;
	wiring_t channel;
#endif
	php_process_id_t child;
	struct php_process_handle *proc;
	int is_persistent = 0; /* TODO: ensure that persistent procs will work */
#ifdef PHP_WIN32
	int suppress_errors = 0;
	int bypass_shell = 0;
#endif
#if PHP_CAN_DO_PTS
	php_file_descriptor_t dev_ptmx = -1;	/* master */
	php_file_descriptor_t slave_pty = -1;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "saz|s!a!a!", &command,
				&command_len, &descriptorspec, &pipes, &cwd, &cwd_len, &environment,
				&other_options) == FAILURE) {
		RETURN_FALSE;
	}

	command = pestrdup(command, is_persistent);

#ifdef PHP_WIN32
	if (other_options) {
		zval **item;
		if (SUCCESS == zend_hash_find(Z_ARRVAL_P(other_options), "suppress_errors", sizeof("suppress_errors"), (void**)&item)) {
			if ((Z_TYPE_PP(item) == IS_BOOL || Z_TYPE_PP(item) == IS_LONG) &&
			    Z_LVAL_PP(item)) {
				suppress_errors = 1;
			}
		}
		if (SUCCESS == zend_hash_find(Z_ARRVAL_P(other_options), "bypass_shell", sizeof("bypass_shell"), (void**)&item)) {
			if ((Z_TYPE_PP(item) == IS_BOOL || Z_TYPE_PP(item) == IS_LONG) &&
			    Z_LVAL_PP(item)) {
				bypass_shell = 1;
			}
		}
	}
#endif

	command_len = strlen(command);

	if (environment) {
		env = _php_array_to_envp(environment, is_persistent TSRMLS_CC);
	} else {
		memset(&env, 0, sizeof(env));
	}

	memset(descriptors, 0, sizeof(descriptors));

#ifdef PHP_WIN32
	/* we use this to allow the child to inherit handles */
	memset(&security, 0, sizeof(security));
	security.nLength = sizeof(security);
	security.bInheritHandle = TRUE;
	security.lpSecurityDescriptor = NULL;
#endif

	/* walk the descriptor spec and set up files/pipes */
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(descriptorspec), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(descriptorspec), (void **)&descitem, &pos) == SUCCESS) {
		char *str_index;
		ulong nindex;
		zval **ztype;

		str_index = NULL;
		zend_hash_get_current_key_ex(Z_ARRVAL_P(descriptorspec), &str_index, NULL, &nindex, 0, &pos);

		if (str_index) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "descriptor spec must be an integer indexed array");
			goto exit_fail;
		}

		descriptors[ndesc].index = nindex;

		if (Z_TYPE_PP(descitem) == IS_RESOURCE) {
			/* should be a stream - try and dup the descriptor */
			php_stream *stream;
			php_socket_t fd;

			php_stream_from_zval(stream, descitem);

			if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&fd, REPORT_ERRORS)) {
				goto exit_fail;
			}

#ifdef PHP_WIN32
			descriptors[ndesc].childend = dup_fd_as_handle(fd);
			if (descriptors[ndesc].childend == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to dup File-Handle for descriptor %d", nindex);
				goto exit_fail;
			}
#else
			descriptors[ndesc].childend = dup(fd);
			if (descriptors[ndesc].childend < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to dup File-Handle for descriptor %ld - %s", nindex, strerror(errno));
				goto exit_fail;
			}
#endif
			descriptors[ndesc].mode = DESC_FILE;

		} else if (Z_TYPE_PP(descitem) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Descriptor item must be either an array or a File-Handle");
			goto exit_fail;
		} else {

			if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 0, (void **)&ztype) == SUCCESS) {
				convert_to_string_ex(ztype);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing handle qualifier in array");
				goto exit_fail;
			}

			if (strcmp(Z_STRVAL_PP(ztype), "pipe") == 0) {
				php_file_descriptor_t newpipe[2];
				zval **zmode;

				if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 1, (void **)&zmode) == SUCCESS) {
					convert_to_string_ex(zmode);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing mode parameter for 'pipe'");
					goto exit_fail;
				}

				descriptors[ndesc].mode = DESC_PIPE;

				if (0 != pipe(newpipe)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to create pipe %s", strerror(errno));
					goto exit_fail;
				}

				if (strncmp(Z_STRVAL_PP(zmode), "w", 1) != 0) {
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
				if (Z_STRLEN_PP(zmode) >= 2 && Z_STRVAL_PP(zmode)[1] == 'b')
					descriptors[ndesc].mode_flags |= O_BINARY;
#endif

			} else if (strcmp(Z_STRVAL_PP(ztype), "file") == 0) {
				zval **zfile, **zmode;
				php_socket_t fd;
				php_stream *stream;

				descriptors[ndesc].mode = DESC_FILE;

				if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 1, (void **)&zfile) == SUCCESS) {
					convert_to_string_ex(zfile);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing file name parameter for 'file'");
					goto exit_fail;
				}

				if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 2, (void **)&zmode) == SUCCESS) {
					convert_to_string_ex(zmode);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing mode parameter for 'file'");
					goto exit_fail;
				}

				/* try a wrapper */
				stream = php_stream_open_wrapper(Z_STRVAL_PP(zfile), Z_STRVAL_PP(zmode),
						REPORT_ERRORS|STREAM_WILL_CAST, NULL);

				/* force into an fd */
				if (stream == NULL || FAILURE == php_stream_cast(stream,
							PHP_STREAM_CAST_RELEASE|PHP_STREAM_AS_FD,
							(void **)&fd, REPORT_ERRORS)) {
					goto exit_fail;
				}

#ifdef PHP_WIN32
				descriptors[ndesc].childend = dup_fd_as_handle(fd);
				_close(fd);

				/* simulate the append mode by fseeking to the end of the file
				this introduces a potential race-condition, but it is the best we can do, though */
				if (strchr(Z_STRVAL_PP(zmode), 'a')) {
					SetFilePointer(descriptors[ndesc].childend, 0, NULL, FILE_END);
				}
#else
				descriptors[ndesc].childend = fd;
#endif
			} else if (strcmp(Z_STRVAL_PP(ztype), "pty") == 0) {
#if PHP_CAN_DO_PTS
				if (dev_ptmx == -1) {
					/* open things up */
					dev_ptmx = open("/dev/ptmx", O_RDWR);
					if (dev_ptmx == -1) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to open /dev/ptmx, errno %d", errno);
						goto exit_fail;
					}
					grantpt(dev_ptmx);
					unlockpt(dev_ptmx);
					slave_pty = open(ptsname(dev_ptmx), O_RDWR);

					if (slave_pty == -1) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to open slave pty, errno %d", errno);
						goto exit_fail;
					}
				}
				descriptors[ndesc].mode = DESC_PIPE;
				descriptors[ndesc].childend = dup(slave_pty);
				descriptors[ndesc].parentend = dup(dev_ptmx);
				descriptors[ndesc].mode_flags = O_RDWR;
#else
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "pty pseudo terminal not supported on this system");
				goto exit_fail;
#endif
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not a valid descriptor spec/mode", Z_STRVAL_PP(ztype));
				goto exit_fail;
			}
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(descriptorspec), &pos);
		if (++ndesc == PHP_PROC_OPEN_MAX_DESCRIPTORS)
			break;
	}

#ifdef PHP_WIN32
	if (cwd == NULL) {
		char *getcwd_result;
		getcwd_result = VCWD_GETCWD(cur_cwd, MAXPATHLEN);
		if (!getcwd_result) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot get current directory");
			goto exit_fail;
		}
		cwd = cur_cwd;
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

	if (bypass_shell) {
		newprocok = CreateProcess(NULL, command, &security, &security, TRUE, dwCreateFlags, env.envp, cwd, &si, &pi);
	} else {
		spprintf(&command_with_cmd, 0, "%s /c %s", COMSPEC_NT, command);

		newprocok = CreateProcess(NULL, command_with_cmd, &security, &security, TRUE, dwCreateFlags, env.envp, cwd, &si, &pi);

		efree(command_with_cmd);
	}

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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "CreateProcess failed, error code - %u", dw);
		goto exit_fail;
	}

	childHandle = pi.hProcess;
	child       = pi.dwProcessId;
	CloseHandle(pi.hThread);

#elif defined(NETWARE)
	if (cwd) {
		orig_cwd = getcwd(NULL, PATH_MAX);
		chdir2(cwd);
	}
	channel.infd = descriptors[0].childend;
	channel.outfd = descriptors[1].childend;
	channel.errfd = -1;
	/* Duplicate the command as processing downwards will modify it*/
	command_dup = strdup(command);
	if (!command_dup) {
		goto exit_fail;
	}
	/* get a number of args */
	construct_argc_argv(command_dup, NULL, &command_num_args, NULL);
	child_argv = (char**) malloc((command_num_args + 1) * sizeof(char*));
	if(!child_argv) {
		free(command_dup);
		if (cwd && orig_cwd) {
			chdir2(orig_cwd);
			free(orig_cwd);
		}
	}
	/* fill the child arg vector */
	construct_argc_argv(command_dup, NULL, &command_num_args, child_argv);
	child_argv[command_num_args] = NULL;
	child = procve(child_argv[0], PROC_DETACHED|PROC_INHERIT_CWD, NULL, &channel, NULL, NULL, 0, NULL, (const char**)child_argv);
	free(child_argv);
	free(command_dup);
	if (cwd && orig_cwd) {
		chdir2(orig_cwd);
		free(orig_cwd);
	}
	if (child < 0) {
		/* failed to fork() */
		/* clean up all the descriptors */
		for (i = 0; i < ndesc; i++) {
			close(descriptors[i].childend);
			if (descriptors[i].parentend)
				close(descriptors[i].parentend);
		}
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "procve failed - %s", strerror(errno));
		goto exit_fail;
	}
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

#if PHP_CAN_DO_PTS
		if (dev_ptmx >= 0) {
			close(dev_ptmx);
			close(slave_pty);
		}
#endif

		if (cwd) {
			php_ignore_value(chdir(cwd));
		}

		if (env.envarray) {
			execle("/bin/sh", "sh", "-c", command, NULL, env.envarray);
		} else {
			execl("/bin/sh", "sh", "-c", command, NULL);
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

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "fork failed - %s", strerror(errno));

		goto exit_fail;

	}
#else
# error You lose (configure should not have let you get here)
#endif
	/* we forked/spawned and this is the parent */

	proc = (struct php_process_handle*)pemalloc(sizeof(struct php_process_handle), is_persistent);
	proc->is_persistent = is_persistent;
	proc->command = command;
	proc->npipes = ndesc;
	proc->child = child;
#ifdef PHP_WIN32
	proc->childHandle = childHandle;
#endif
	proc->env = env;

	if (pipes != NULL) {
		zval_dtor(pipes);
	}
	array_init(pipes);

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
#else
				stream = php_stream_fopen_from_fd(descriptors[i].parentend, mode_string, NULL);
# if defined(F_SETFD) && defined(FD_CLOEXEC)
				/* mark the descriptor close-on-exec, so that it won't be inherited by potential other children */
				fcntl(descriptors[i].parentend, F_SETFD, FD_CLOEXEC);
# endif
#endif
				if (stream) {
					zval *retfp;

					/* nasty hack; don't copy it */
					stream->flags |= PHP_STREAM_FLAG_NO_SEEK;

					MAKE_STD_ZVAL(retfp);
					php_stream_to_zval(stream, retfp);
					add_index_zval(pipes, descriptors[i].index, retfp);

					proc->pipes[i] = Z_LVAL_P(retfp);
				}
				break;
			default:
				proc->pipes[i] = 0;
		}
	}

	ZEND_REGISTER_RESOURCE(return_value, proc, le_proc_open);
	return;

exit_fail:
	_php_free_envp(env, is_persistent);
	pefree(command, is_persistent);
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
