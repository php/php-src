/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include "php.h"
#include <ctype.h>
#include "php_string.h"
#include "safe_mode.h"
#include "ext/standard/head.h"
#include "ext/standard/file.h"
#include "exec.h"
#include "php_globals.h"
#include "SAPI.h"

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

/* This symbol is defined in ext/standard/config.m4.
 * Essentially, it is set if you HAVE_FORK || PHP_WIN32
 * Otherplatforms may modify that configure check and add suitable #ifdefs
 * around the alternate code.
 * */
#ifdef PHP_CAN_SUPPORT_PROC_OPEN

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
		
		convert_to_string_ex(element);
		el_len = Z_STRLEN_PP(element);
		if (el_len == 0) {
			continue;
		}
		
		sizeenv += el_len+1;
		
		switch (zend_hash_get_current_key_ex(target_hash, &string_key, &string_length, &num_key, 0, &pos)) {
			case HASH_KEY_IS_STRING:
				if (string_length == 0) {
					continue;
				}
				sizeenv += string_length+1;
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
		
		convert_to_string_ex(element);
		el_len = Z_STRLEN_PP(element);
		
		if (el_len == 0) {
			continue;
		}
		
		data = Z_STRVAL_PP(element);
		switch (zend_hash_get_current_key_ex(target_hash, &string_key, &string_length, &num_key, 0, &pos)) {
			case HASH_KEY_IS_STRING:
				if (string_length == 0) {
					continue;
				}
				l = string_length + el_len + 1;
				memcpy(p, string_key, string_length);
				strcat(p, "=");
				strcat(p, data);
				
				if (PG(magic_quotes_gpc)) {
					php_stripslashes(p, &l TSRMLS_CC);
				}
#ifndef PHP_WIN32
				*ep = p;
				++ep;
#endif
				p += l;
				break;
			case HASH_KEY_IS_LONG:
				memcpy(p,data,el_len);
				if (PG(magic_quotes_gpc)) {
					php_stripslashes(p, &el_len TSRMLS_CC);
				}
#ifndef PHP_WIN32
				*ep = p;
				++ep;
#endif
				p += el_len + 1;
				break;
			case HASH_KEY_NON_EXISTANT:
				break;
		}
	}	

	assert(p - env.envp <= sizeenv);
	
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


static void proc_open_rsrc_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct php_process_handle *proc = (struct php_process_handle*)rsrc->ptr;
	int i;
#ifdef PHP_WIN32
	DWORD wstatus;
#elif HAVE_SYS_WAIT_H
	int wstatus;
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
	
	WaitForSingleObject(proc->child, INFINITE);
	GetExitCodeProcess(proc->child, &wstatus);
	FG(pclose_ret) = wstatus;
	
#elif HAVE_SYS_WAIT_H
	
	do {
		wait_pid = waitpid(proc->child, &wstatus, 0);
	} while (wait_pid == -1 && errno == EINTR);
	
	if (wait_pid == -1)
		FG(pclose_ret) = -1;
	else {
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

/* {{{ php_make_safe_mode_command */
static int php_make_safe_mode_command(char *cmd, char **safecmd, int is_persistent TSRMLS_DC)
{
	int lcmd, larg0, ldir, len, overflow_limit;
	char *space, *sep, *arg0;

	if (!PG(safe_mode)) {
		*safecmd = pestrdup(cmd, is_persistent);
		return SUCCESS;
	}

	lcmd = strlen(cmd);
	ldir = strlen(PG(safe_mode_exec_dir));
	len = lcmd + ldir + 2;
	overflow_limit = len;

	arg0 = emalloc(len);
	
	strcpy(arg0, cmd);
	
	space = strchr(arg0, ' ');
	if (space) {
		*space = '\0';
	}
	larg0 = strlen(arg0);

	if (strstr(arg0, "..")) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No '..' components allowed in path");
		efree(arg0);
		return FAILURE;
	}

	*safecmd = emalloc(len);
	strcpy(*safecmd, PG(safe_mode_exec_dir));
	overflow_limit -= ldir;
	
	sep = strrchr(arg0, PHP_DIR_SEPARATOR);
	if (sep) {
		strcat(*safecmd, sep);
		overflow_limit -= strlen(sep);
	} else {
		strcat(*safecmd, "/");
		strcat(*safecmd, arg0);
		overflow_limit -= larg0 + 1;
	}
	if (space) {
		strncat(*safecmd, cmd + larg0, overflow_limit);
	}
	efree(arg0);
	arg0 = php_escape_shell_cmd(*safecmd);
	efree(*safecmd);
	if (is_persistent) {
		*safecmd = pestrdup(arg0, 1);
		efree(arg0);
	} else {
		*safecmd = arg0;
	}

	return SUCCESS;
}
/* }}} */

PHP_MINIT_FUNCTION(proc_open)
{
	le_proc_open = zend_register_list_destructors_ex(proc_open_rsrc_dtor, NULL, "process", module_number);
	return SUCCESS;
}


/* {{{ proto int proc_terminate(resource process [, long signal])
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
	TerminateProcess(proc->child, 255);
#else
	kill(proc->child, sig_no);
#endif
	
	zend_list_delete(Z_LVAL_P(zproc));
	RETURN_LONG(FG(pclose_ret));
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
	
	zend_list_delete(Z_LVAL_P(zproc));
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
	
	GetExitCodeProcess(proc->child, &wstatus);

	running = wstatus == STILL_ACTIVE;
	exitcode == STILL_ACTIVE ? -1 : wstatus;
	
#elif HAVE_SYS_WAIT_H
	
	errno = 0;
	wait_pid = waitpid(proc->child, &wstatus, WNOHANG|WUNTRACED);
	
	if (wait_pid == proc->child) {
		if (WIFEXITED(wstatus)) {
			running = 0;
			exitcode = WEXITSTATUS(wstatus);
		}
		if (WIFSIGNALED(wstatus)) {
			signaled = 1;
			termsig = WTERMSIG(wstatus);
		}
		if (WIFSTOPPED(wstatus)) {
			stopped = 1;
			stopsig = WSTOPSIG(wstatus);
		}
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
# define pipe(pair)		(CreatePipe(&pair[0], &pair[1], &security, 2048L) ? 0 : -1)

# define COMSPEC_NT	"cmd.exe"
# define COMSPEC_9X	"command.com"

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

/* {{{ proto resource proc_open(string command, array descriptorspec, array &pipes [, string cwd [, array env]])
   Run a process with more control over it's file descriptors */
PHP_FUNCTION(proc_open)
{

	char *command, *cwd=NULL;
	long command_len, cwd_len;
	zval *descriptorspec;
	zval *pipes;
	zval *environment = NULL;
	php_process_env_t env;
	int ndesc = 0;
	int i;
	zval **descitem = NULL;
	HashPosition pos;
	struct php_proc_open_descriptor_item descriptors[PHP_PROC_OPEN_MAX_DESCRIPTORS];
#ifdef PHP_WIN32
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL newprocok;
	SECURITY_ATTRIBUTES security;
	char *command_with_cmd;
#endif
	php_process_id_t child;
	struct php_process_handle *proc;
	int is_persistent = 0; /* TODO: ensure that persistent procs will work */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "saz/|s!a!", &command,
				&command_len, &descriptorspec, &pipes, &cwd, &cwd_len, &environment) == FAILURE) {
		RETURN_FALSE;
	}

	if (FAILURE == php_make_safe_mode_command(command, &command, is_persistent TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (cwd_len==0) {
		cwd = NULL;
	}

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
			int fd;

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
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to dup File-Handle for descriptor %d - %s", nindex, strerror(errno));
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
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing mode parameter for 'pipe'", Z_STRVAL_PP(ztype));
					goto exit_fail;
				}

				descriptors[ndesc].mode = DESC_PIPE;

				if (0 != pipe(newpipe)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to create pipe %s", strerror(errno));
					goto exit_fail;
				}

				if (strcmp(Z_STRVAL_PP(zmode), "w") != 0) {
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
				int fd;
				php_stream *stream;

				descriptors[ndesc].mode = DESC_FILE;

				if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 1, (void **)&zfile) == SUCCESS) {
					convert_to_string_ex(zfile);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing file name parameter for 'file'", Z_STRVAL_PP(ztype));
					goto exit_fail;
				}

				if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 2, (void **)&zmode) == SUCCESS) {
					convert_to_string_ex(zmode);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing mode parameter for 'file'", Z_STRVAL_PP(ztype));
					goto exit_fail;
				}

				/* try a wrapper */
				stream = php_stream_open_wrapper(Z_STRVAL_PP(zfile), Z_STRVAL_PP(zmode),
						ENFORCE_SAFE_MODE|REPORT_ERRORS|STREAM_WILL_CAST, NULL);

				/* force into an fd */
				if (stream == NULL || FAILURE == php_stream_cast(stream,
							PHP_STREAM_CAST_RELEASE|PHP_STREAM_AS_FD,
							(void **)&fd, REPORT_ERRORS)) {
					goto exit_fail;
				}

#ifdef PHP_WIN32
				descriptors[ndesc].childend = (HANDLE)_get_osfhandle(fd);
#else
				descriptors[ndesc].childend = fd;
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
	
	command_with_cmd = emalloc(command_len + sizeof(COMSPEC_9X) + 1 + sizeof(" /c "));
	sprintf(command_with_cmd, "%s /c %s", GetVersion() < 0x80000000 ? COMSPEC_NT : COMSPEC_9X, command);
	newprocok = CreateProcess(NULL, command_with_cmd, &security, &security, TRUE, NORMAL_PRIORITY_CLASS, env.envp, cwd, &si, &pi);
	efree(command_with_cmd);

	if (FALSE == newprocok) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "CreateProcess failed");
		goto exit_fail;
	}

	child = pi.hProcess;
	CloseHandle(pi.hThread);

#elif HAVE_FORK
	/* the unix way */
	child = fork();

	if (child == 0) {
		/* this is the child process */

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
			chdir(cwd);
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
	proc->env = env;

	array_init(pipes);

	/* clean up all the child ends and then open streams on the parent
	 * ends, where appropriate */
	for (i = 0; i < ndesc; i++) {
		FILE *fp;
		char *mode_string=NULL;
		php_stream *stream;

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
				}
#ifdef PHP_WIN32
				fp = _fdopen(_open_osfhandle((long)descriptors[i].parentend,
							descriptors[i].mode_flags), mode_string);
#else
				fp = fdopen(descriptors[i].parentend, mode_string);
#endif
				if (fp) {
					stream = php_stream_fopen_from_file(fp, mode_string);
					if (stream) {
						zval *retfp;

						MAKE_STD_ZVAL(retfp);
						php_stream_to_zval(stream, retfp);
						add_index_zval(pipes, descriptors[i].index, retfp);

						proc->pipes[i] = Z_LVAL_P(retfp);
					}
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
