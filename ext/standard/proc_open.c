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
   | Author: Wez Furlong                                                  |
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

static int le_proc_open;

static void proc_open_rsrc_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
#ifdef PHP_WIN32
	HANDLE child;
	DWORD wstatus;
	
	child = (HANDLE)rsrc->ptr;
	WaitForSingleObject(child, INFINITE);
	GetExitCodeProcess(child, &wstatus);
	FG(pclose_ret) = wstatus;
#else
# if HAVE_SYS_WAIT_H
	int wstatus;
	pid_t child, wait_pid;
	
	child = (pid_t)rsrc->ptr;

	do {
		wait_pid = waitpid(child, &wstatus, 0);
	} while (wait_pid == -1 && errno == EINTR);
	
	if (wait_pid == -1)
		FG(pclose_ret) = -1;
	else {
		if (WIFEXITED(wstatus))
			wstatus = WEXITSTATUS(wstatus);
		FG(pclose_ret) = wstatus;
	}
	
# else
	FG(pclose_ret) = -1;
# endif
#endif
}

/* {{{ php_make_safe_mode_command */
static int php_make_safe_mode_command(char *cmd, char **safecmd TSRMLS_DC)
{
	int lcmd, larg0, ldir, len, overflow_limit;
	char *space, *sep, *arg0;

	if (!PG(safe_mode)) {
		*safecmd = estrdup(cmd);
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
	*safecmd = arg0;

	return SUCCESS;
}
/* }}} */



PHP_MINIT_FUNCTION(proc_open)
{
	le_proc_open = zend_register_list_destructors_ex(proc_open_rsrc_dtor, NULL, "process", module_number);
	return SUCCESS;
}

/* {{{ proto int proc_close(resource process)
   close a process opened by proc_open */
PHP_FUNCTION(proc_close)
{
	zval *proc;
	void *child;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &proc) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(child, void *, &proc, -1, "process", le_proc_open);
	
	zend_list_delete(Z_LVAL_P(proc));
	RETURN_LONG(FG(pclose_ret));
}
/* }}} */

/* {{{ handy definitions for portability/readability */
#ifdef PHP_WIN32
typedef HANDLE descriptor_t;
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
typedef int descriptor_t;
# define close_descriptor(fd)	close(fd)

#endif

#define DESC_PIPE		1
#define DESC_FILE		2
#define DESC_PARENT_MODE_WRITE	8

struct php_proc_open_descriptor_item {
	int index; 							/* desired fd number in child process */
	descriptor_t parentend, childend;	/* fds for pipes in parent/child */
	int mode;							/* mode for proc_open code */
	int mode_flags;						/* mode flags for opening fds */
};
/* }}} */

/* {{{ proto resource proc_open(string command, array descriptorspec, array &pipes)
   Run a process with more control over it's file descriptors */
PHP_FUNCTION(proc_open)
{
#define MAX_DESCRIPTORS	16

	char *command;
	long command_len;
	zval *descriptorspec;
	zval *pipes;
	int ndesc = 0;
	int i;
	zval **descitem = NULL;
	HashPosition pos;
	struct php_proc_open_descriptor_item descriptors[MAX_DESCRIPTORS];
#ifdef PHP_WIN32
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL newprocok;
	HANDLE child;
	SECURITY_ATTRIBUTES security;
	char *command_with_cmd;
#else
	pid_t child;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "saz/", &command,
				&command_len, &descriptorspec, &pipes) == FAILURE) {
		RETURN_FALSE;
	}

	if (FAILURE == php_make_safe_mode_command(command, &command TSRMLS_CC)) {
		RETURN_FALSE;
	}

	command_len = strlen(command);
	
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
				descriptor_t newpipe[2];
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
				size_t old_size = FG(def_chunk_size);

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

				FG(def_chunk_size) = 1;
				stream = php_stream_open_wrapper(Z_STRVAL_PP(zfile), Z_STRVAL_PP(zmode),
						ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);
				FG(def_chunk_size) = old_size;

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
		if (++ndesc == MAX_DESCRIPTORS)
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
	newprocok = CreateProcess(NULL, command_with_cmd, &security, &security, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	efree(command_with_cmd);

	if (FALSE == newprocok) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "CreateProcess failed");
		goto exit_fail;
	}

	child = pi.hProcess;
	CloseHandle(pi.hThread);

#else
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

		execl("/bin/sh", "sh", "-c", command, NULL);
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
#endif
	/* we forked/spawned and this is the parent */

	efree(command);
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
					}
				}
				break;
		}
	}

	ZEND_REGISTER_RESOURCE(return_value, (void*)child, le_proc_open);
	return;

exit_fail:
	efree(command);
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
