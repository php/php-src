/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf                                               |
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

/* {{{ php_Exec
 * If type==0, only last line of output is returned (exec)
 * If type==1, all lines will be printed and last lined returned (system)
 * If type==2, all lines will be saved to given array (exec with &$array)
 * If type==3, output will be printed binary, no lines will be saved or returned (passthru)
 *
 */
int php_Exec(int type, char *cmd, pval *array, pval *return_value TSRMLS_DC)
{
	FILE *fp;
	char *buf, *tmp=NULL;
	int buflen = 0;
	int t, l, output=1;
	int overflow_limit, lcmd, ldir;
	char *b, *c, *d=NULL;
	php_stream *stream = NULL;
#if PHP_SIGCHILD
	void (*sig_handler)();
#endif

	buf = (char *) emalloc(EXEC_INPUT_BUF);
	if (!buf) {
		php_error(E_WARNING, "Unable to emalloc %d bytes for exec buffer", EXEC_INPUT_BUF);
		return -1;
	}
	buflen = EXEC_INPUT_BUF;

	if (PG(safe_mode)) {
		lcmd = strlen(cmd);
		ldir = strlen(PG(safe_mode_exec_dir));
		l = lcmd + ldir + 2;
		overflow_limit = l;
		c = strchr(cmd, ' ');
		if (c) *c = '\0';
		if (strstr(cmd, "..")) {
			php_error(E_WARNING, "No '..' components allowed in path");
			efree(buf);
			return -1;
		}
		d = emalloc(l);
		strcpy(d, PG(safe_mode_exec_dir));
		overflow_limit -= ldir;
		b = strrchr(cmd, PHP_DIR_SEPARATOR);
		if (b) {
			strcat(d, b);
			overflow_limit -= strlen(b);
		} else {
			strcat(d, "/");
			strcat(d, cmd);
			overflow_limit-=(strlen(cmd)+1);
		}
		if (c) {
			*c = ' ';
			strncat(d, c, overflow_limit);
		}
		tmp = php_escape_shell_cmd(d);
		efree(d);
		d = tmp;
#if PHP_SIGCHILD
		sig_handler = signal (SIGCHLD, SIG_DFL);
#endif
#ifdef PHP_WIN32
		fp = VCWD_POPEN(d, "rb");
#else
		fp = VCWD_POPEN(d, "r");
#endif
		if (!fp) {
			php_error(E_WARNING, "Unable to fork [%s]", d);
			efree(d);
			efree(buf);
#if PHP_SIGCHILD
			signal (SIGCHLD, sig_handler);
#endif
			return -1;
		}

	} else { /* not safe_mode */
#if PHP_SIGCHILD
		sig_handler = signal (SIGCHLD, SIG_DFL);
#endif
#ifdef PHP_WIN32
		fp = VCWD_POPEN(cmd, "rb");
#else
		fp = VCWD_POPEN(cmd, "r");
#endif
		if (!fp) {
			php_error(E_WARNING, "Unable to fork [%s]", cmd);
			efree(buf);
#if PHP_SIGCHILD
			signal (SIGCHLD, sig_handler);
#endif
			return -1;
		}
	}
	buf[0] = '\0';
	if (type==2) {
		if (Z_TYPE_P(array) != IS_ARRAY) {
			pval_destructor(array);
			array_init(array);
		}
	}

	/* we register the resource so that case of an aborted connection the 
	 * fd gets pclosed
	 */

	stream = php_stream_fopen_from_pipe(fp, "rb");

	if (type != 3) {
		l=0;
		while ( !feof(fp) || l != 0 ) {
			l = 0;
			/* Read a line or fill the buffer, whichever comes first */
			do {
				if ( buflen <= (l+1) ) {
					buf = erealloc(buf, buflen + EXEC_INPUT_BUF);
					if ( buf == NULL ) {
						php_error(E_WARNING, "Unable to erealloc %d bytes for exec buffer", 
								buflen + EXEC_INPUT_BUF);
#if PHP_SIGCHILD
						signal (SIGCHLD, sig_handler);
#endif
						return -1;
					}
					buflen += EXEC_INPUT_BUF;
				}

				if ( fgets(&(buf[l]), buflen - l, fp) == NULL ) {
					/* eof */
					break;
				}
				l += strlen(&(buf[l]));
			} while ( (l > 0) && (buf[l-1] != '\n') );

			if ( feof(fp) && (l == 0) ) {
				break;
			}

		
			if (type == 1) {
				if (output) PUTS(buf);
				sapi_flush(TSRMLS_C);
			}
			else if (type == 2) {
				/* strip trailing whitespaces */	
				l = strlen(buf);
				t = l;
				while (l-- && isspace((int)buf[l]));
				if (l < t) {
					buf[l + 1] = '\0';
				}
				add_next_index_string(array, buf, 1);
			}
		}

		/* strip trailing spaces */
		l = strlen(buf);
		t = l;
		while (l && isspace((int)buf[l - 1])) {
			l--;
		}
		if (l < t) buf[l] = '\0';

		/* Return last line from the shell command */
		if (PG(magic_quotes_runtime)) {
			int len;

			tmp = php_addslashes(buf, 0, &len, 0 TSRMLS_CC);
			RETVAL_STRINGL(tmp, len, 0);
		} else {
			RETVAL_STRINGL(buf, l, 1);
		}
	} else {
		int b, i;

		while ((b = fread(buf, 1, buflen, fp)) > 0) {
			for (i = 0; i < b; i++)
				if (output) (void)PUTC(buf[i]);
		}
	}

	php_stream_close(stream); 

#if HAVE_SYS_WAIT_H
	if (WIFEXITED(FG(pclose_ret))) {
		FG(pclose_ret) = WEXITSTATUS(FG(pclose_ret));
	}
#endif
#if PHP_SIGCHILD
	signal (SIGCHLD, sig_handler);
#endif
	if (d) {
		efree(d);
	}
	efree(buf);
	return FG(pclose_ret);
}
/* }}} */

/* {{{ proto string exec(string command [, array output [, int return_value]])
   Execute an external program */
PHP_FUNCTION(exec)
{
	pval **arg1, **arg2, **arg3;
	int arg_count = ZEND_NUM_ARGS();
	int ret;

	if (arg_count > 3 || zend_get_parameters_ex(arg_count, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (arg_count) {
		case 1:
			ret = php_Exec(0, Z_STRVAL_PP(arg1), NULL, return_value TSRMLS_CC);
			break;
		case 2:
			ret = php_Exec(2, Z_STRVAL_PP(arg1), *arg2, return_value TSRMLS_CC);
			break;
		case 3:
			ret = php_Exec(2, Z_STRVAL_PP(arg1), *arg2, return_value TSRMLS_CC);
			Z_TYPE_PP(arg3) = IS_LONG;
			Z_LVAL_PP(arg3)=ret;
			break;
	}
}

/* }}} */

/* {{{ proto int system(string command [, int return_value])
   Execute an external program and display output */
PHP_FUNCTION(system)
{
	pval **arg1, **arg2;
	int arg_count = ZEND_NUM_ARGS();
	int ret;

	if (arg_count > 2 || zend_get_parameters_ex(arg_count, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (arg_count) {
		case 1:
			ret = php_Exec(1, Z_STRVAL_PP(arg1), NULL, return_value TSRMLS_CC);
			break;
		case 2:
			ret = php_Exec(1, Z_STRVAL_PP(arg1), NULL, return_value TSRMLS_CC);
			Z_TYPE_PP(arg2) = IS_LONG;
			Z_LVAL_PP(arg2)=ret;
			break;
	}
}
/* }}} */

/* {{{ proto void passthru(string command [, int return_value])
   Execute an external program and display raw output */
PHP_FUNCTION(passthru)
{
	pval **arg1, **arg2;
	int arg_count = ZEND_NUM_ARGS();
	int ret;

	if (arg_count > 2 || zend_get_parameters_ex(arg_count, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (arg_count) {
		case 1:
			ret = php_Exec(3, Z_STRVAL_PP(arg1), NULL, return_value TSRMLS_CC);
			break;
		case 2:
			ret = php_Exec(3, Z_STRVAL_PP(arg1), NULL, return_value TSRMLS_CC);
			Z_TYPE_PP(arg2) = IS_LONG;
			Z_LVAL_PP(arg2)=ret;
			break;
	}
}
/* }}} */

/* {{{ php_escape_shell_cmd
   Escape all chars that could possibly be used to
   break out of a shell command

   This function emalloc's a string and returns the pointer.
   Remember to efree it when done with it.

   *NOT* safe for binary strings
*/
char *php_escape_shell_cmd(char *str) {
	register int x, y, l;
	char *cmd;

	l = strlen(str);
	cmd = emalloc(2 * l + 1);
	
	for (x = 0, y = 0; x < l; x++) {
		switch (str[x]) {
			case '#': /* This is character-set independent */
			case '&':
			case ';':
			case '`':
			case '\'':
			case '"':
			case '|':
			case '*':
			case '?':
			case '~':
			case '<':
			case '>':
			case '^':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '$':
			case '\\':
			case '\x0A': /* excluding these two */
			case '\xFF':
				cmd[y++] = '\\';
				/* fall-through */
			default:
				cmd[y++] = str[x];

		}
	}
	cmd[y] = '\0';
	return cmd;
}
/* }}} */

/* {{{ php_escape_shell_arg
 */
char *php_escape_shell_arg(char *str) {
	int x, y, l;
	char *cmd;

	y = 0;
	l = strlen(str);
	
	cmd = emalloc(4 * l + 3); /* worst case */
	
	cmd[y++] = '\'';
	
	for (x = 0; x < l; x++) {
		switch (str[x]) {
		case '\'':
			cmd[y++] = '\'';
			cmd[y++] = '\\';
			cmd[y++] = '\'';
			/* fall-through */
		default:
			cmd[y++] = str[x];
		}
	}
	cmd[y++] = '\'';
	cmd[y] = '\0';
	return cmd;
}
/* }}} */

/* {{{ proto string escapeshellcmd(string command)
   Escape shell metacharacters */
PHP_FUNCTION(escapeshellcmd)
{
	pval **arg1;
	char *cmd = NULL;

	if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(arg1);
	if (Z_STRLEN_PP(arg1)) {
		cmd = php_escape_shell_cmd(Z_STRVAL_PP(arg1));
		RETVAL_STRING(cmd, 1);
		efree(cmd);
	}
}
/* }}} */

/* {{{ proto string escapeshellarg(string arg)
   Quote and escape an argument for use in a shell command */
PHP_FUNCTION(escapeshellarg)
{
	pval **arg1;
	char *cmd = NULL;

	if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(arg1);
	if (Z_STRLEN_PP(arg1)) {
		cmd = php_escape_shell_arg(Z_STRVAL_PP(arg1));
		RETVAL_STRING(cmd, 1);
		efree(cmd);
	}
}
/* }}} */

/* {{{ proto string shell_exec(string cmd)
   Use pclose() for FILE* that has been opened via popen() */
PHP_FUNCTION(shell_exec)
{
	FILE *in;
	int readbytes, total_readbytes=0, allocated_space;
	pval **cmd;
	char *ret;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &cmd)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (PG(safe_mode)) {
		php_error(E_WARNING, "Cannot execute using backquotes in safe mode");
		RETURN_FALSE;
	}

	convert_to_string_ex(cmd);
#ifdef PHP_WIN32
	if ((in=VCWD_POPEN(Z_STRVAL_PP(cmd), "rt"))==NULL) {
#else
	if ((in=VCWD_POPEN(Z_STRVAL_PP(cmd), "r"))==NULL) {
#endif
		php_error(E_WARNING, "Unable to execute '%s'", Z_STRVAL_PP(cmd));
	}
	allocated_space = EXEC_INPUT_BUF;
	ret = (char *) emalloc(allocated_space);
	while (1) {
		readbytes = fread(ret+total_readbytes, 1, EXEC_INPUT_BUF, in);
		if (readbytes<=0) {
			break;
		}
		total_readbytes += readbytes;
		allocated_space = total_readbytes+EXEC_INPUT_BUF;
		ret = (char *) erealloc(ret, allocated_space);
	}
	pclose(in);
	
	RETVAL_STRINGL(ret, total_readbytes, 0);
	Z_STRVAL_P(return_value)[total_readbytes] = '\0';	
}
/* }}} */

static int le_proc_open;

static void proc_open_rsrc_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
#if HAVE_SYS_WAIT
	int wstatus;
	pid_t child, wait_pid;
	
	child = (pid_t)rsrc->ptr;

	do {
		wait_pid = waitpid(child, &wstatus, 0);
	} while (wait_pid == -1 && errno = EINTR);
	
	if (wait_pid == -1)
		FG(pclose_ret) = -1;
	else
		FG(pclose_ret) = wstatus;
#else
	FG(pclose_ret) = -1;
#endif
}

PHP_MINIT_FUNCTION(proc_open)
{
	le_proc_open = zend_register_list_destructors_ex(proc_open_rsrc_dtor, NULL, "proc-opened-process", module_number);
	return SUCCESS;
}

/* {{{ proto resource proc_open(string command, array descriptorspec, array &pipes)
   Run a process with more control over it's file descriptors */
PHP_FUNCTION(proc_open)
{
#define MAX_DESCRIPTORS	16
#define DESC_PIPE		1
#define DESC_FILE		2
#define DESC_PARENT_MODE_WRITE	8
#ifndef O_BINARY
#define O_BINARY	0
#endif

#ifdef PHP_WIN32
	/* NOINHERIT is actually required for pipes to work correctly when inherited;
	 * see the win32 specific dup call a little further down */
#define pipe(handles)	_pipe((handles), 512, _O_BINARY | _O_NOINHERIT)
#endif

	char *command;
	long command_len;
	zval *descriptorspec;
	zval *pipes;
	struct {
		int index; /* desired fd number in child process */
		int parentend, childend; /* fds for pipes in parent/child */
		int mode;
	} descriptors[MAX_DESCRIPTORS];
	int ndesc = 0;
	int i;
	zval **descitem = NULL;
	HashPosition pos;
#ifdef PHP_WIN32
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL newprocok;
	HANDLE child;
#else
	pid_t child;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "saz/", &command,
				&command_len, &descriptorspec, &pipes) == FAILURE) {
		RETURN_FALSE;
	}

	memset(descriptors, 0, sizeof(descriptors));

	/* walk the descriptor spec and set up files/pipes */
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(descriptorspec), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(descriptorspec), (void **)&descitem, &pos) == SUCCESS) {
		char *str_index;
		ulong nindex;
		zval **ztype;

		str_index = NULL;
		zend_hash_get_current_key_ex(Z_ARRVAL_P(descriptorspec), &str_index, NULL, &nindex, 0, &pos);

		if (str_index) {
			zend_error(E_WARNING, "%s(): descriptor spec must be an integer indexed array",
					get_active_function_name(TSRMLS_C));
			RETURN_FALSE;
		}

		descriptors[ndesc].index = nindex;

		if (Z_TYPE_PP(descitem) == IS_RESOURCE) {
			/* should be a stream - try and dup the descriptor */
			php_stream *stream;
			int fd;

			ZEND_FETCH_RESOURCE(stream, php_stream *, descitem, -1, "File-Handle", php_file_le_stream());

			if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&fd, REPORT_ERRORS)) {
				RETURN_FALSE;
			}

			descriptors[ndesc].childend = dup(fd);
			if (descriptors[ndesc].childend < 0) {
				zend_error(E_WARNING, "%s(): unable to dup File-Handle for descriptor %d - %s",
						get_active_function_name(TSRMLS_C), nindex, strerror(errno));
			}
			descriptors[ndesc].mode = DESC_FILE;

		} else if (Z_TYPE_PP(descitem) != IS_ARRAY) {
			zend_error(E_WARNING, "%s(): descriptor item must be either an array or a File-Handle",
					get_active_function_name(TSRMLS_C));
			RETURN_FALSE;
		} else {

			zend_hash_index_find(Z_ARRVAL_PP(descitem), 0, (void **)&ztype);
			convert_to_string_ex(ztype);

			if (strcmp(Z_STRVAL_PP(ztype), "pipe") == 0) {
				int newpipe[2];
				zval **zmode;

				descriptors[ndesc].mode = DESC_PIPE;
				if (0 != pipe(newpipe)) {
					zend_error(E_WARNING, "%s(): unable to create pipe %s",
							get_active_function_name(TSRMLS_C), strerror(errno));
					RETURN_FALSE;
				}

				zend_hash_index_find(Z_ARRVAL_PP(descitem), 1, (void **)&zmode);
				convert_to_string_ex(zmode);

				if (strcmp(Z_STRVAL_PP(zmode), "w") != 0) {
					descriptors[ndesc].parentend = newpipe[1];
					descriptors[ndesc].childend = newpipe[0];
					descriptors[ndesc].mode |= DESC_PARENT_MODE_WRITE;
				} else {
					descriptors[ndesc].parentend = newpipe[0];
					descriptors[ndesc].childend = newpipe[1];
				}

#ifdef PHP_WIN32
				{	/* this magic is needed to ensure that pipes work
					 * correctly in the child */
					int tmpfd = dup(descriptors[ndesc].childend);
					close(descriptors[ndesc].childend);
					descriptors[ndesc].childend = tmpfd;
				}
#endif

			} else if (strcmp(Z_STRVAL_PP(ztype), "file") == 0) {
				zval **zfile, **zmode;
				int open_flags = 0;
				const struct {
					char *modestring;
					int flags;
				} modemap[] = {
					{ "r",		O_RDONLY },
					{ "rb",		O_RDONLY | O_BINARY },
					{ "r+",		O_RDWR },
					{ "rb+",	O_RDWR | O_BINARY },
					{ "w",		O_WRONLY | O_CREAT | O_TRUNC },
					{ "wb",		O_WRONLY | O_CREAT | O_TRUNC | O_BINARY},
					{ "w+",		O_RDWR | O_CREAT | O_TRUNC },
					{ "wb+",	O_RDWR | O_CREAT | O_TRUNC | O_BINARY},
					{ "a",		O_WRONLY | O_CREAT | O_APPEND },
					{ "ab",		O_WRONLY | O_CREAT | O_APPEND | O_BINARY },
					{ "a+",		O_RDWR | O_CREAT | O_APPEND },
					{ "ab+",	O_RDWR | O_CREAT | O_APPEND | O_BINARY},
					{ NULL, 0 }
				};

				descriptors[ndesc].mode = DESC_FILE;

				zend_hash_index_find(Z_ARRVAL_PP(descitem), 1, (void **)&zfile);
				convert_to_string_ex(zfile);

				zend_hash_index_find(Z_ARRVAL_PP(descitem), 2, (void **)&zmode);
				convert_to_string_ex(zmode);

				for (i = 0; modemap[i].modestring != NULL; i++) {
					if (strcmp(modemap[i].modestring, Z_STRVAL_PP(zmode)) == 0) {
						open_flags = modemap[i].flags;
						break;
					}
				}

				descriptors[ndesc].childend = VCWD_OPEN_MODE(Z_STRVAL_PP(zfile), open_flags, 0644);
				if (descriptors[ndesc].childend < 0) {

					if (PG(allow_url_fopen)) {
						/* try a wrapper */
						int fd;
						php_stream *stream;
						size_t old_size = FG(def_chunk_size);

						FG(def_chunk_size) = 1;
						stream = php_stream_open_wrapper(Z_STRVAL_PP(zfile), Z_STRVAL_PP(zmode),
								ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);
						FG(def_chunk_size) = old_size;

						if (stream == NULL)
							RETURN_FALSE;

						/* force into an fd */
						if (FAILURE == php_stream_cast(stream, PHP_STREAM_CAST_RELEASE|PHP_STREAM_AS_FD, (void **)&fd, REPORT_ERRORS))
							RETURN_FALSE;

						descriptors[ndesc].childend = fd;

					} else {

						zend_error(E_WARNING, "%s(): unable to open %s with mode %s",
								get_active_function_name(TSRMLS_C),
								Z_STRVAL_PP(zfile), Z_STRVAL_PP(zmode));
						RETURN_FALSE;
					}
				}
			} else {
				zend_error(E_WARNING, "%s(): %s is not a valid descriptor spec/mode",
						get_active_function_name(TSRMLS_C), Z_STRVAL_PP(ztype));
				RETURN_FALSE;
			}
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(descriptorspec), &pos);
		if (++ndesc == MAX_DESCRIPTORS)
			break;
	}

#ifdef PHP_WIN32
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	
	/* redirect stdin/stdout/stderr if requested */
	for (i = 0; i < ndesc; i++) {
		switch(descriptors[i].index) {
			case 0:
				si.hStdInput = (HANDLE)_get_osfhandle(descriptors[i].childend);
				si.dwFlags |= STARTF_USESTDHANDLES;
				break;
			case 1:
				si.hStdOutput = (HANDLE)_get_osfhandle(descriptors[i].childend);
				si.dwFlags |= STARTF_USESTDHANDLES;
				break;
			case 2:
				si.hStdError = (HANDLE)_get_osfhandle(descriptors[i].childend);
				si.dwFlags |= STARTF_USESTDHANDLES;
				break;
		}
	}

	memset(&pi, 0, sizeof(pi));
	
	newprocok = CreateProcess(NULL, command, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

	if (FALSE == newprocok) {
		zend_error(E_WARNING, "%s(): CreateProcess failed", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	child = pi.hProcess;
	
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

		zend_error(E_WARNING, "%s(): fork failed - %s",
				get_active_function_name(TSRMLS_C),
				strerror(errno)
				);

		RETURN_FALSE;

	}
#endif
	/* we forked/spawned and this is the parent */

	array_init(pipes);

	/* clean up all the child ends and then open streams on the parent
	 * ends, where appropriate */
	for (i = 0; i < ndesc; i++) {
		char *stdiomode;
		FILE *fp;
		php_stream *stream;

		close(descriptors[i].childend);

		switch (descriptors[i].mode & ~DESC_PARENT_MODE_WRITE) {
			case DESC_PIPE:
				stdiomode = descriptors[i].mode & DESC_PARENT_MODE_WRITE ? "w" : "r";
				fp = fdopen(descriptors[i].parentend, stdiomode);
				if (fp) {
					stream = php_stream_fopen_from_file(fp, stdiomode);
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

	ZEND_REGISTER_RESOURCE(return_value, child, le_proc_open);

}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
