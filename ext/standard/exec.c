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
		zend_error(E_WARNING, "No '..' components allowed in path");
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
		php_error(E_WARNING, "%s(): Unable to emalloc %d bytes for exec buffer", get_active_function_name(TSRMLS_C), EXEC_INPUT_BUF);
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
			php_error(E_WARNING, "%s(): No '..' components allowed in path", get_active_function_name(TSRMLS_C));
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
			php_error(E_WARNING, "%s(): Unable to fork [%s]", get_active_function_name(TSRMLS_C), d);
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
			php_error(E_WARNING, "%s(): Unable to fork [%s]", get_active_function_name(TSRMLS_C), cmd);
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
						php_error(E_WARNING, "%s(): Unable to erealloc %d bytes for exec buffer", get_active_function_name(TSRMLS_C), 
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

	FG(pclose_ret) = php_stream_close(stream); 

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
		php_error(E_WARNING, "%s(): Cannot execute using backquotes in Safe Mode", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	convert_to_string_ex(cmd);
#ifdef PHP_WIN32
	if ((in=VCWD_POPEN(Z_STRVAL_PP(cmd), "rt"))==NULL) {
#else
	if ((in=VCWD_POPEN(Z_STRVAL_PP(cmd), "r"))==NULL) {
#endif
		php_error(E_WARNING, "%s(): Unable to execute '%s'", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(cmd));
		RETURN_FALSE;
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

/* {{{ proc_open resource management */
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
# if HAVE_SYS_WAIT
	int wstatus;
	pid_t child, wait_pid;
	
	child = (pid_t)rsrc->ptr;

	do {
		wait_pid = waitpid(child, &wstatus, 0);
	} while (wait_pid == -1 && errno == EINTR);
	
	if (wait_pid == -1)
		FG(pclose_ret) = -1;
	else
		FG(pclose_ret) = wstatus;
# else
	FG(pclose_ret) = -1;
# endif
#endif
}

PHP_MINIT_FUNCTION(proc_open)
{
	le_proc_open = zend_register_list_destructors_ex(proc_open_rsrc_dtor, NULL, "process", module_number);
	return SUCCESS;
}

/* }}} */

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
			zend_error(E_WARNING, "%s(): descriptor spec must be an integer indexed array",
					get_active_function_name(TSRMLS_C));
			goto exit_fail;
		}

		descriptors[ndesc].index = nindex;

		if (Z_TYPE_PP(descitem) == IS_RESOURCE) {
			/* should be a stream - try and dup the descriptor */
			php_stream *stream;
			int fd;

			ZEND_FETCH_RESOURCE(stream, php_stream *, descitem, -1, "File-Handle", php_file_le_stream());

			if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&fd, REPORT_ERRORS)) {
				goto exit_fail;
			}

#ifdef PHP_WIN32
			descriptors[ndesc].childend = dup_fd_as_handle(fd);
			if (descriptors[ndesc].childend == NULL) {
				zend_error(E_WARNING, "%s(): unable to dup File-Handle for descriptor %d",
						get_active_function_name(TSRMLS_C), nindex);
				goto exit_fail;
			}
#else
			descriptors[ndesc].childend = dup(fd);
			if (descriptors[ndesc].childend < 0) {
				zend_error(E_WARNING, "%s(): unable to dup File-Handle for descriptor %d - %s",
						get_active_function_name(TSRMLS_C), nindex, strerror(errno));
				goto exit_fail;
			}
#endif
			descriptors[ndesc].mode = DESC_FILE;

		} else if (Z_TYPE_PP(descitem) != IS_ARRAY) {
			php_error(E_WARNING, "%s(): Descriptor item must be either an array or a File-Handle",
					get_active_function_name(TSRMLS_C));
			goto exit_fail;
		} else {

			if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 0, (void **)&ztype) == SUCCESS) {
				convert_to_string_ex(ztype);
			} else {
				php_error (E_WARNING, "%s(): Missing handle qualifier in array",
					get_active_function_name(TSRMLS_C), Z_STRVAL_PP(ztype));
				goto exit_fail;
			}

			if (strcmp(Z_STRVAL_PP(ztype), "pipe") == 0) {
				descriptor_t newpipe[2];
				zval **zmode;

				if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 1, (void **)&zmode) == SUCCESS) {
					convert_to_string_ex(zmode);
				} else {
					php_error (E_WARNING, "%s(): Missing mode parameter for 'pipe'",
						get_active_function_name(TSRMLS_C), Z_STRVAL_PP(ztype));
					goto exit_fail;
				}

				descriptors[ndesc].mode = DESC_PIPE;

				if (0 != pipe(newpipe)) {
					zend_error(E_WARNING, "%s(): unable to create pipe %s",
							get_active_function_name(TSRMLS_C), strerror(errno));
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
					php_error (E_WARNING, "%s(): Missing file name parameter for 'file'",
						get_active_function_name(TSRMLS_C), Z_STRVAL_PP(ztype));
					goto exit_fail;
				}

				if (zend_hash_index_find(Z_ARRVAL_PP(descitem), 2, (void **)&zmode) == SUCCESS) {
					convert_to_string_ex(zmode);
				} else {
					php_error (E_WARNING, "%s(): Missing mode parameter for 'file'",
						get_active_function_name(TSRMLS_C), Z_STRVAL_PP(ztype));
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
				zend_error(E_WARNING, "%s(): %s is not a valid descriptor spec/mode",
						get_active_function_name(TSRMLS_C), Z_STRVAL_PP(ztype));
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
		zend_error(E_WARNING, "%s(): CreateProcess failed", get_active_function_name(TSRMLS_C));
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

		zend_error(E_WARNING, "%s(): fork failed - %s",
				get_active_function_name(TSRMLS_C),
				strerror(errno)
				);

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
		char *mode_string;
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
