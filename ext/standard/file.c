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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Stig Bakken <ssb@fast.no>                                   |
   |          Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   | PHP 4.0 patches by Thies C. Arntzen (thies@thieso.net)               |
   | PHP streams by Wez Furlong (wez@thebrainroom.com)                    |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Synced with php 3.0 revision 1.218 1999-06-16 [ssb] */

/* {{{ includes */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/flock_compat.h"
#include "ext/standard/exec.h"
#include "ext/standard/php_filestat.h"
#include "php_open_temporary_file.h"
#include "ext/standard/basic_functions.h"
#include "php_ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef PHP_WIN32
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#include "win32/winutil.h"
#elif defined(NETWARE) && !defined(NEW_LIBC)
/*#include <ws2nlm.h>*/
#include <sys/socket.h>
#include "netware/param.h"
#else
#include <sys/param.h>
#include <sys/select.h>
#if defined(NETWARE) && defined(USE_WINSOCK)
#include <novsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif
#include "ext/standard/head.h"
#include "safe_mode.h"
#include "php_string.h"
#include "file.h"
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#elif defined(NETWARE)
#include "netware/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include "fsock.h"
#include "fopen_wrappers.h"
#include "php_streams.h"
#include "php_globals.h"

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#if MISSING_FCLOSE_DECL
extern int fclose(FILE *);
#endif

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

#include "scanf.h"
#include "zend_API.h"

#ifdef ZTS
int file_globals_id;
#else
php_file_globals file_globals;
#endif

#ifdef HAVE_FNMATCH
#include <fnmatch.h>
#endif

/* }}} */
/* {{{ ZTS-stuff / Globals / Prototypes */

/* sharing globals is *evil* */
static int le_stream_context = FAILURE;

/* }}} */
/* {{{ Module-Stuff */

static ZEND_RSRC_DTOR_FUNC(file_context_dtor)
{
	php_stream_context_free((php_stream_context*)rsrc->ptr);
}

static void file_globals_ctor(php_file_globals *file_globals_p TSRMLS_DC)
{
	FG(pclose_ret) = 0;
	FG(user_stream_current_filename) = NULL;
	FG(def_chunk_size) = PHP_SOCK_CHUNK_SIZE;
}

static void file_globals_dtor(php_file_globals *file_globals_p TSRMLS_DC)
{
}


PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("user_agent", NULL, PHP_INI_ALL, OnUpdateString, user_agent, php_file_globals, file_globals)
	STD_PHP_INI_ENTRY("default_socket_timeout", "60", PHP_INI_ALL, OnUpdateInt, default_socket_timeout, php_file_globals, file_globals)
	STD_PHP_INI_ENTRY("auto_detect_line_endings", "0", PHP_INI_ALL, OnUpdateInt, auto_detect_line_endings, php_file_globals, file_globals)
PHP_INI_END()

PHP_MINIT_FUNCTION(file)
{
	le_stream_context = zend_register_list_destructors_ex(file_context_dtor, NULL, "stream-context", module_number);

#ifdef ZTS
	ts_allocate_id(&file_globals_id, sizeof(php_file_globals), (ts_allocate_ctor) file_globals_ctor, (ts_allocate_dtor) file_globals_dtor);
#else
	file_globals_ctor(&file_globals TSRMLS_CC);
#endif

	REGISTER_INI_ENTRIES();
	
	REGISTER_LONG_CONSTANT("SEEK_SET", SEEK_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SEEK_CUR", SEEK_CUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SEEK_END", SEEK_END, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_SH", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_EX", 2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_UN", 3, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_NB", 4, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_CONNECT", 		PHP_STREAM_NOTIFY_CONNECT,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_AUTH_REQUIRED",	PHP_STREAM_NOTIFY_AUTH_REQUIRED,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_AUTH_RESULT",		PHP_STREAM_NOTIFY_AUTH_RESULT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_MIME_TYPE_IS",	PHP_STREAM_NOTIFY_MIME_TYPE_IS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_FILE_SIZE_IS",	PHP_STREAM_NOTIFY_FILE_SIZE_IS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_REDIRECTED",		PHP_STREAM_NOTIFY_REDIRECTED,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_PROGRESS",		PHP_STREAM_NOTIFY_PROGRESS,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_FAILURE",			PHP_STREAM_NOTIFY_FAILURE,			CONST_CS | CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_SEVERITY_INFO",	PHP_STREAM_NOTIFY_SEVERITY_INFO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_SEVERITY_WARN",	PHP_STREAM_NOTIFY_SEVERITY_WARN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_SEVERITY_ERR",	PHP_STREAM_NOTIFY_SEVERITY_ERR,  CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_FILTER_READ",			PHP_STREAM_FILTER_READ,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_FILTER_WRITE",			PHP_STREAM_FILTER_WRITE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_FILTER_ALL",				PHP_STREAM_FILTER_ALL,				CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILE_USE_INCLUDE_PATH",	PHP_FILE_USE_INCLUDE_PATH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILE_IGNORE_NEW_LINES",	PHP_FILE_IGNORE_NEW_LINES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILE_SKIP_EMPTY_LINES",	PHP_FILE_SKIP_EMPTY_LINES, CONST_CS | CONST_PERSISTENT);
	
#ifdef HAVE_FNMATCH
	REGISTER_LONG_CONSTANT("FNM_NOESCAPE", FNM_NOESCAPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FNM_PATHNAME", FNM_PATHNAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FNM_PERIOD",   FNM_PERIOD,   CONST_CS | CONST_PERSISTENT);
#ifdef FNM_CASEFOLD /* a GNU extension */ /* TODO emulate if not available */
	REGISTER_LONG_CONSTANT("FNM_CASEFOLD", FNM_CASEFOLD, CONST_CS | CONST_PERSISTENT);
#endif
#endif

	return SUCCESS;
}

/* }}} */

PHP_MSHUTDOWN_FUNCTION(file)
{
#ifndef ZTS
	file_globals_dtor(&file_globals TSRMLS_CC);
#endif
	return SUCCESS;
}



/* {{{ proto bool flock(resource fp, int operation [, int &wouldblock])
   Portable file locking */

static int flock_values[] = { LOCK_SH, LOCK_EX, LOCK_UN };

PHP_FUNCTION(flock)
{
	zval *arg1, *arg3;
	int fd, act, ret;
	php_stream *stream;
	long operation = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|z", &arg1, &operation, &arg3) == FAILURE) {
		return;
	}

	php_stream_from_zval(stream, &arg1);

	if (php_stream_cast(stream, PHP_STREAM_AS_FD, (void*)&fd, 1) == FAILURE) {
		RETURN_FALSE;
	}

	act = operation & 3;
	if (act < 1 || act > 3) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal operation argument");
		RETURN_FALSE;
	}

	/* flock_values contains all possible actions if (operation & 4) we won't block on the lock */
	act = flock_values[act - 1] | (operation & 4 ? LOCK_NB : 0);
	if ((ret = flock(fd, act)) == -1) {
		if (errno == EWOULDBLOCK && operation) {
			convert_to_long(arg3);
			Z_LVAL_P(arg3) = 1;
			RETURN_TRUE;
		}
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */

#define PHP_META_UNSAFE ".\\+*?[^]$() "

/* {{{ proto array get_meta_tags(string filename [, bool use_include_path])
   Extracts all meta tag content attributes from a file and returns an array */

PHP_FUNCTION(get_meta_tags)
{
	char *filename;
	int filename_len;
	zend_bool use_include_path = 0;
	int in_tag = 0, done = 0;
	int looking_for_val = 0, have_name = 0, have_content = 0;
	int saw_name = 0, saw_content = 0;
	char *name = NULL, *value = NULL, *temp = NULL;
	php_meta_tags_token tok, tok_last;
	php_meta_tags_data md;

	/* Initiailize our structure */
	memset(&md, 0, sizeof(md));

	/* Parse arguments */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b",
							  &filename, &filename_len, &use_include_path) == FAILURE) {
		return;
	}

	md.stream = php_stream_open_wrapper(filename, "rb",
			(use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS,
			NULL);

	if (!md.stream)	{
		RETURN_FALSE;
	}

	array_init(return_value);

	tok_last = TOK_EOF;

	while (!done && (tok = php_next_meta_token(&md TSRMLS_CC)) != TOK_EOF) {
		if (tok == TOK_ID) {
			if (tok_last == TOK_OPENTAG) {
				md.in_meta = !strcasecmp("meta", md.token_data);
			} else if (tok_last == TOK_SLASH && in_tag) {
				if (strcasecmp("head", md.token_data) == 0) {
					/* We are done here! */
					done = 1;
				}
			} else if (tok_last == TOK_EQUAL && looking_for_val) {
				if (saw_name) {
					/* Get the NAME attr (Single word attr, non-quoted) */
					temp = name = estrndup(md.token_data, md.token_len);

					while (temp && *temp) {
						if (strchr(PHP_META_UNSAFE, *temp)) {
							*temp = '_';
						}
						temp++;
					}

					have_name = 1;
				} else if (saw_content) {
					/* Get the CONTENT attr (Single word attr, non-quoted) */
					if (PG(magic_quotes_runtime)) {
						value = php_addslashes(md.token_data, 0, &md.token_len, 0 TSRMLS_CC);
					} else {
						value = estrndup(md.token_data, md.token_len);
					}

					have_content = 1;
				}

				looking_for_val = 0;
			} else {
				if (md.in_meta) {
					if (strcasecmp("name", md.token_data) == 0) {
						saw_name = 1;
						saw_content = 0;
						looking_for_val = 1;
					} else if (strcasecmp("content", md.token_data) == 0) {
						saw_name = 0;
						saw_content = 1;
						looking_for_val = 1;
					}
				}
			}
		} else if (tok == TOK_STRING && tok_last == TOK_EQUAL && looking_for_val) {
			if (saw_name) {
				/* Get the NAME attr (Quoted single/double) */
				temp = name = estrndup(md.token_data, md.token_len);

				while (temp && *temp) {
					if (strchr(PHP_META_UNSAFE, *temp)) {
						*temp = '_';
					}
					temp++;
				}

				have_name = 1;
			} else if (saw_content) {
				/* Get the CONTENT attr (Single word attr, non-quoted) */
				if (PG(magic_quotes_runtime)) {
					value = php_addslashes(md.token_data, 0, &md.token_len, 0 TSRMLS_CC);
				} else {
					value = estrndup(md.token_data, md.token_len);
				}

				have_content = 1;
			}

			looking_for_val = 0;
		} else if (tok == TOK_OPENTAG) {
			if (looking_for_val) {
				looking_for_val = 0;
				have_name = saw_name = 0;
				have_content = saw_content = 0;
			}
			in_tag = 1;
		} else if (tok == TOK_CLOSETAG) {
			if (have_name) {
				/* For BC */
				php_strtolower(name, strlen(name));
				if (have_content) {
					add_assoc_string(return_value, name, value, 0); 
				} else {
					add_assoc_string(return_value, name, empty_string, 0);
				}

				efree(name);
			} else if (have_content) {
				efree(value);
			}

			name = value = NULL;
				
			/* Reset all of our flags */
			in_tag = looking_for_val = 0;
			have_name = saw_name = 0;
			have_content = saw_content = 0;
			md.in_meta = 0;
		}

		tok_last = tok;

		if (md.token_data)
			efree(md.token_data);

		md.token_data = NULL;
	}

	php_stream_close(md.stream);
}

/* }}} */

/* {{{ proto string file_get_contents(string filename [, bool use_include_path])
   Read the entire file into a string */
PHP_FUNCTION(file_get_contents)
{
	char *filename;
	int filename_len;
	char *contents;
	zend_bool use_include_path = 0;
	php_stream *stream;
	int len, newlen;

	/* Parse arguments */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b",
							  &filename, &filename_len, &use_include_path) == FAILURE) {
		return;
	}

	stream = php_stream_open_wrapper(filename, "rb", 
			(use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS,
			NULL);
	if (!stream) {
		RETURN_FALSE;
	}

	/* uses mmap if possible */
	if ((len = php_stream_copy_to_mem(stream, &contents, PHP_STREAM_COPY_ALL, 0)) >= 0) {
		
		if (PG(magic_quotes_runtime)) {
			contents = php_addslashes(contents, len, &newlen, 1 TSRMLS_CC); /* 1 = free source string */
			len = newlen;
		}

		RETVAL_STRINGL(contents, len, 0);
	} else {
		RETVAL_FALSE;
	}

	php_stream_close(stream);
	
}
/* }}} */

/* {{{ proto array file(string filename [, int flags])
   Read entire file into an array */

#define PHP_FILE_BUF_SIZE	80

PHP_FUNCTION(file)
{
	char *filename;
	int filename_len;
	char *slashed, *target_buf=NULL, *p, *s, *e;
	register int i = 0;
	int target_len, len;
	char eol_marker = '\n';
	long flags = 0;
	zend_bool use_include_path;
	zend_bool include_new_line;
	zend_bool skip_blank_lines;
	php_stream *stream;

	/* Parse arguments */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &filename, &filename_len, &flags) == FAILURE) {
		return;
	}
	if (flags < 0 || flags > (PHP_FILE_USE_INCLUDE_PATH | PHP_FILE_IGNORE_NEW_LINES | PHP_FILE_SKIP_EMPTY_LINES)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%l' flag is not supported.", flags);
		RETURN_FALSE;
	}
	
	use_include_path = flags & PHP_FILE_USE_INCLUDE_PATH;
	include_new_line = !(flags & PHP_FILE_IGNORE_NEW_LINES);
	skip_blank_lines = flags & PHP_FILE_SKIP_EMPTY_LINES;

	stream = php_stream_open_wrapper(filename, "rb", (use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL);
	if (!stream) {
		RETURN_FALSE;
	}

	/* Initialize return array */
	array_init(return_value);

 	if ((target_len = php_stream_copy_to_mem(stream, &target_buf, PHP_STREAM_COPY_ALL, 0))) {
 		s = target_buf;
 		e = target_buf + target_len;
 	
 		if (!(p = php_stream_locate_eol(stream, target_buf, target_len TSRMLS_CC))) {
 			p = e;
 			goto parse_eol;
  		}
  		
 		if (stream->flags & PHP_STREAM_FLAG_EOL_MAC) {
  			eol_marker = '\r';
 		}	

		/* for performance reasons the code is duplicated, so that the if (include_new_line) 
		 * will not need to be done for every single line in the file.
		 */
		if (include_new_line) {	
	 		do {
 				p++;
 				parse_eol:
 				if (PG(magic_quotes_runtime)) {
 					/* s is in target_buf which is freed at the end of the function */
 					slashed = php_addslashes(s, (p-s), &len, 0 TSRMLS_CC);
 					add_index_stringl(return_value, i++, slashed, len, 0);
 				} else {
 					add_index_stringl(return_value, i++, estrndup(s, p-s), p-s, 0);
  				}
 				s = p;
	 		} while ((p = memchr(p, eol_marker, (e-p))));
	 	} else {
	 		do {
 				if (skip_blank_lines && !(p-s)) {
 					s = ++p;
 					continue;
 				}
 				if (PG(magic_quotes_runtime)) {
 					/* s is in target_buf which is freed at the end of the function */
 					slashed = php_addslashes(s, (p-s), &len, 0 TSRMLS_CC);
 					add_index_stringl(return_value, i++, slashed, len, 0);
 				} else {
 					add_index_stringl(return_value, i++, estrndup(s, p-s), p-s, 0);
  				}
 				s = ++p;
	 		} while ((p = memchr(p, eol_marker, (e-p))));
	 	}
 		
 		/* handle any left overs of files without new lines */
 		if (s != e) {
 			p = e;
 			goto parse_eol;
  		}
  	}

 	if (target_buf) {
 		efree(target_buf);
 	}	
	php_stream_close(stream);
}
/* }}} */

/* {{{ proto string tempnam(string dir, string prefix)
   Create a unique filename in a directory */
PHP_FUNCTION(tempnam)
{
	pval **arg1, **arg2;
	char *d;
	char *opened_path;
	char p[64];
	FILE *fp;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);

	if (php_check_open_basedir(Z_STRVAL_PP(arg1) TSRMLS_CC)) {
		RETURN_FALSE;
	}
	
	d = estrndup(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1));
	strlcpy(p, Z_STRVAL_PP(arg2), sizeof(p));

	if ((fp = php_open_temporary_file(d, p, &opened_path TSRMLS_CC))) {
		fclose(fp);
		RETVAL_STRING(opened_path, 0);
	} else {
		RETVAL_FALSE;
	}
	efree(d);
}
/* }}} */

/* {{{ proto resource tmpfile(void)
   Create a temporary file that will be deleted automatically after use */
PHP_NAMED_FUNCTION(php_if_tmpfile)
{
	php_stream *stream;
	
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	stream = php_stream_fopen_tmpfile();

	if (stream)	{
		php_stream_to_zval(stream, return_value);
	}
	else	{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource stream_get_meta_data(resource fp)
    Retrieves header/meta data from streams/file pointers */
PHP_FUNCTION(stream_get_meta_data)
{
	zval **arg1;
	php_stream *stream;
	zval *newval;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	php_stream_from_zval(stream, arg1);

	array_init(return_value);
	
	if (stream->wrapperdata) {
		MAKE_STD_ZVAL(newval);
		*newval = *(stream->wrapperdata);
		zval_copy_ctor(newval);

		add_assoc_zval(return_value, "wrapper_data", newval);
	}
	if (stream->wrapper) {
		add_assoc_string(return_value, "wrapper_type", (char *)stream->wrapper->wops->label, 1);
	}
	add_assoc_string(return_value, "stream_type", (char *)stream->ops->label, 1);

#if 0	/* TODO: needs updating for new filter API */
	if (stream->filterhead) {
		php_stream_filter *filter;
		
		MAKE_STD_ZVAL(newval);
		array_init(newval);
		
		for (filter = stream->filterhead; filter != NULL; filter = filter->next) {
			add_next_index_string(newval, (char *)filter->fops->label, 1);
		}

		add_assoc_zval(return_value, "filters", newval);
	}
#endif
	
	add_assoc_long(return_value, "unread_bytes", stream->writepos - stream->readpos);
	
	if (php_stream_is(stream, PHP_STREAM_IS_SOCKET))	{
		php_netstream_data_t *sock = PHP_NETSTREAM_DATA_FROM_STREAM(stream);

		add_assoc_bool(return_value, "timed_out", sock->timeout_event);
		add_assoc_bool(return_value, "blocked", sock->is_blocked);
		add_assoc_bool(return_value, "eof", stream->eof);
	} else {
		add_assoc_bool(return_value, "timed_out", 0);
		add_assoc_bool(return_value, "blocked", 1);
		add_assoc_bool(return_value, "eof", php_stream_eof(stream));
	}

}
/* }}} */

/* {{{ proto array stream_get_wrappers()
    Retrieves list of registered stream wrappers */
PHP_FUNCTION(stream_get_wrappers)
{
	HashTable *url_stream_wrappers_hash;
	char *stream_protocol;
	int key_flags, stream_protocol_len = 0;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	if ((url_stream_wrappers_hash = php_stream_get_url_stream_wrappers_hash())) {
		array_init(return_value);
		for(zend_hash_internal_pointer_reset(url_stream_wrappers_hash);
			(key_flags = zend_hash_get_current_key_ex(url_stream_wrappers_hash, &stream_protocol, &stream_protocol_len, NULL, 0, NULL)) != HASH_KEY_NON_EXISTANT;
			zend_hash_move_forward(url_stream_wrappers_hash)) {
				if (key_flags == HASH_KEY_IS_STRING) {
					add_next_index_stringl(return_value, stream_protocol, stream_protocol_len, 1);
				}
		}
	} else {
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ stream_select related functions */
static int stream_array_to_fd_set(zval *stream_array, fd_set *fds, int *max_fd TSRMLS_DC)
{
	zval **elem;
	php_stream *stream;
	int this_fd;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(stream_array), (void **) &elem) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(stream_array))) {

		php_stream_from_zval_no_verify(stream, elem);
		if (stream == NULL) {
			continue;
		}
		/* get the fd.
		 * NB: Most other code will NOT use the PHP_STREAM_CAST_INTERNAL flag
		 * when casting.  It is only used here so that the buffered data warning
		 * is not displayed.
		 * */
		if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1)) {
			FD_SET(this_fd, fds);
			if (this_fd > *max_fd) {
				*max_fd = this_fd;
			}
		}
	}
	return 1;
}

static int stream_array_from_fd_set(zval *stream_array, fd_set *fds TSRMLS_DC)
{
	zval **elem, **dest_elem;
	php_stream *stream;
	HashTable *new_hash;
	int this_fd, ret = 0;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, 0, NULL, ZVAL_PTR_DTOR, 0);
	
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(stream_array), (void **) &elem) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(stream_array))) {

		php_stream_from_zval_no_verify(stream, elem);
		if (stream == NULL) {
			continue;
		}
		/* get the fd 
		 * NB: Most other code will NOT use the PHP_STREAM_CAST_INTERNAL flag
		 * when casting.  It is only used here so that the buffered data warning
		 * is not displayed.
		 */
		if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD | PHP_STREAM_CAST_INTERNAL, (void*)&this_fd, 1)) {
			if (FD_ISSET(this_fd, fds)) {
				zend_hash_next_index_insert(new_hash, (void *)elem, sizeof(zval *), (void **)&dest_elem);
				if (dest_elem) {
					zval_add_ref(dest_elem);
				}
				ret++;
				continue;
			}
		}
	}

	/* destroy old array and add new one */
	zend_hash_destroy(Z_ARRVAL_P(stream_array));
	efree(Z_ARRVAL_P(stream_array));

	zend_hash_internal_pointer_reset(new_hash);
	Z_ARRVAL_P(stream_array) = new_hash;
	
	return ret;
}

static int stream_array_emulate_read_fd_set(zval *stream_array TSRMLS_DC)
{
	zval **elem, **dest_elem;
	php_stream *stream;
	HashTable *new_hash;
	int ret = 0;

	if (Z_TYPE_P(stream_array) != IS_ARRAY) {
		return 0;
	}
	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, 0, NULL, ZVAL_PTR_DTOR, 0);
	
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(stream_array), (void **) &elem) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(stream_array))) {

		php_stream_from_zval_no_verify(stream, elem);
		if (stream == NULL) {
			continue;
		}
		if ((stream->writepos - stream->readpos) > 0) {
			/* allow readable non-descriptor based streams to participate in stream_select.
			 * Non-descriptor streams will only "work" if they have previously buffered the
			 * data.  Not ideal, but better than nothing.
			 * This branch of code also allows blocking streams with buffered data to
			 * operate correctly in stream_select.
			 * */
			zend_hash_next_index_insert(new_hash, (void *)elem, sizeof(zval *), (void **)&dest_elem);
			if (dest_elem) {
				zval_add_ref(dest_elem);
			}
			ret++;
			continue;
		}
	}

	if (ret > 0) {
		/* destroy old array and add new one */
		zend_hash_destroy(Z_ARRVAL_P(stream_array));
		efree(Z_ARRVAL_P(stream_array));

		zend_hash_internal_pointer_reset(new_hash);
		Z_ARRVAL_P(stream_array) = new_hash;
	} else {
		zend_hash_destroy(new_hash);
		FREE_HASHTABLE(new_hash);
	}
	
	return ret;
}
/* }}} */

#ifdef PHP_WIN32
/* Win32 select() will only work with sockets, so we roll our own implementation that will
 * get the OS file handle from regular fd's and sockets and then use WaitForMultipleObjects().
 * This implementation is not as feature-full as posix select, but it works for our purposes
 */
static int php_select(int max_fd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tv)
{
	HANDLE *handles;
	DWORD waitret;
	DWORD ms_total;
	int i, f, s, fd_count = 0, sock_count = 0;
	int retval;
	fd_set ard, awr, aex; /* active fd sets */

	for (i = 0; i < max_fd; i++) {
		if (FD_ISSET(i, rfds) || FD_ISSET(i, wfds) || FD_ISSET(i, efds)) {
			if (_get_osfhandle(i) == 0xffffffff) {
				/* it is a socket */
				sock_count++;
			} else {
				fd_count++;
			}
		}
	}

	if (fd_count + sock_count == 0) {
		return 0;
	}

	handles = (HANDLE*)emalloc((fd_count + sock_count) * sizeof(HANDLE));

	/* populate the events and handles arrays */
	f = 0;
	s = 0;
	for (i = 0; i < max_fd; i++) {
		if (FD_ISSET(i, rfds) || FD_ISSET(i, wfds) || FD_ISSET(i, efds)) {
			long h = _get_osfhandle(i);
			if (h == 0xFFFFFFFF) {
				HANDLE evt;
				long evt_flags = 0;

				if (FD_ISSET(i, rfds)) {
					evt_flags |= FD_READ|FD_CONNECT|FD_ACCEPT|FD_CLOSE;
				}
				if (FD_ISSET(i, wfds)) {
					evt_flags |= FD_WRITE;
				}
				if (FD_ISSET(i, efds)) {
					evt_flags |= FD_OOB;
				}

				evt = WSACreateEvent();
				WSAEventSelect(i, evt, evt_flags); 

				handles[fd_count + s] = evt;
				s++;
			} else {
				handles[f++] = (HANDLE)h;
			}
		}
	}

	/* calculate how long we need to wait in milliseconds */
	if (tv == NULL) {
		ms_total = INFINITE;
	} else {
		ms_total = tv->tv_sec * 1000;
		ms_total += tv->tv_usec / 1000;
	}

	waitret = MsgWaitForMultipleObjects(fd_count + sock_count, handles, FALSE, ms_total, QS_ALLEVENTS);

	if (waitret == WAIT_TIMEOUT) {
		retval = 0;
	} else if (waitret == 0xFFFFFFFF) {
		retval = -1;
	} else {

		FD_ZERO(&ard);
		FD_ZERO(&awr);
		FD_ZERO(&aex);

		f = 0;
		retval = 0;
		for (i = 0; i < max_fd; i++) {
			if (FD_ISSET(i, rfds) || FD_ISSET(i, wfds) || FD_ISSET(i, efds)) {
				if (f >= fd_count) {
					/* socket event */
					HANDLE evt = handles[f];

					if (WAIT_OBJECT_0 == WaitForSingleObject(evt, 0)) {
						/* check for various signal states */
						if (FD_ISSET(i, rfds)) {
							WSAEventSelect(i, evt, FD_READ|FD_CONNECT|FD_ACCEPT|FD_CLOSE);
							if (WAIT_OBJECT_0 == WaitForSingleObject(evt, 0)) {
								FD_SET(i, &ard);
							}
						}
						if (FD_ISSET(i, wfds)) {
							WSAEventSelect(i, evt, FD_WRITE);
							if (WAIT_OBJECT_0 == WaitForSingleObject(evt, 0)) {
								FD_SET(i, &awr);
							}
						}
						if (FD_ISSET(i, efds)) {
							WSAEventSelect(i, evt, FD_OOB);
							if (WAIT_OBJECT_0 == WaitForSingleObject(evt, 0)) {
								FD_SET(i, &aex);
							}
						}
						retval++;
					}

					WSACloseEvent(evt);

				} else {
					if (WAIT_OBJECT_0 == WaitForSingleObject(handles[f], 0)) {
						if (FD_ISSET(i, rfds)) {
							FD_SET(i, &ard);
						}
						if (FD_ISSET(i, wfds)) {
							FD_SET(i, &awr);
						}
						if (FD_ISSET(i, efds)) {
							FD_SET(i, &aex);
						}
						retval++;
					}

				}
				f++;
			}
		}

		if (rfds) {
			*rfds = ard;
		}
		if (wfds) {
			*wfds = awr;
		}
		if (efds) {
			*efds = aex;
		}
	}

	efree(handles);

	return retval;
}
#else
#define php_select(m, r, w, e, t)	select(m, r, w, e, t)
#endif

/* {{{ proto int stream_select(array &read_streams, array &write_streams, array &except_streams, int tv_sec[, int tv_usec])
   Runs the select() system call on the sets of streams with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(stream_select)
{
	zval			*r_array, *w_array, *e_array, *sec;
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	int				max_fd = 0;
	int				retval, sets = 0, usec = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a!a!a!z!|l", &r_array, &w_array, &e_array, &sec, &usec) == FAILURE)
		return;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) sets += stream_array_to_fd_set(r_array, &rfds, &max_fd TSRMLS_CC);
	if (w_array != NULL) sets += stream_array_to_fd_set(w_array, &wfds, &max_fd TSRMLS_CC);
	if (e_array != NULL) sets += stream_array_to_fd_set(e_array, &efds, &max_fd TSRMLS_CC);

	if (!sets) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No stream arrays were passed");
		RETURN_FALSE;
	}

	/* If seconds is not set to null, build the timeval, else we wait indefinitely */
	if (sec != NULL) {
		convert_to_long_ex(&sec);
		tv.tv_sec = Z_LVAL_P(sec);
		tv.tv_usec = usec;
		tv_p = &tv;
	}

	/* slight hack to support buffered data; if there is data sitting in the
	 * read buffer of any of the streams in the read array, let's pretend
	 * that we selected, but return only the readable sockets */
	if (r_array != NULL) {

		retval = stream_array_emulate_read_fd_set(r_array TSRMLS_CC);
		if (retval > 0) {
			RETURN_LONG(retval);
		}
	}
	
	retval = php_select(max_fd+1, &rfds, &wfds, &efds, tv_p);

	if (retval == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to select [%d]: %s (max_fd=%d)",
				errno, strerror(errno), max_fd);
		RETURN_FALSE;
	}

	if (r_array != NULL) stream_array_from_fd_set(r_array, &rfds TSRMLS_CC);
	if (w_array != NULL) stream_array_from_fd_set(w_array, &wfds TSRMLS_CC);
	if (e_array != NULL) stream_array_from_fd_set(e_array, &efds TSRMLS_CC);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ stream_context related functions */
static void user_space_stream_notifier(php_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr TSRMLS_DC)
{
	zval *callback = (zval*)context->notifier->ptr;
	zval *retval = NULL;
	zval zvs[6];
	zval *ps[6];
	zval **ptps[6];
	int i;
	
	for (i = 0; i < 6; i++) {
		INIT_ZVAL(zvs[i]);
		ps[i] = &zvs[i];
		ptps[i] = &ps[i];
	}
		
	ZVAL_LONG(ps[0], notifycode);
	ZVAL_LONG(ps[1], severity);
	if (xmsg) {
		ZVAL_STRING(ps[2], xmsg, 0);
	} else {
		ZVAL_NULL(ps[2]);
	}
	ZVAL_LONG(ps[3], xcode);
	ZVAL_LONG(ps[4], bytes_sofar);
	ZVAL_LONG(ps[5], bytes_max);

	if (FAILURE == call_user_function_ex(EG(function_table), NULL, callback, &retval, 6, ptps, 0, NULL TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to call user notifier");
	}
	if (retval) {
		zval_ptr_dtor(&retval);
	}
}

static int parse_context_options(php_stream_context *context, zval *options)
{
	HashPosition pos, opos;
	zval **wval, **oval;
	char *wkey, *okey;
	int wkey_len, okey_len;
	int ret = SUCCESS;
	
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(options), &pos);
	while (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_P(options), (void**)&wval, &pos)) {
		if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_P(options), &wkey, &wkey_len, NULL, 0, &pos)
				&& Z_TYPE_PP(wval) == IS_ARRAY) {

			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(wval), &opos);
			while (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(wval), (void**)&oval, &opos)) {

				if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_PP(wval), &okey, &okey_len, NULL, 0, &opos)) {
					ZVAL_ADDREF(*oval);
					php_stream_context_set_option(context, wkey, okey, *oval);
				}
				zend_hash_move_forward_ex(Z_ARRVAL_PP(wval), &opos);
			}

		} else {
			zend_error(E_WARNING, "options should have the form [\"wrappername\"][\"optionname\"] = $value");
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(options), &pos);
	}

	return ret;
}

static int parse_context_params(php_stream_context *context, zval *params)
{
	int ret = SUCCESS;
	zval **tmp;

	if (SUCCESS == zend_hash_find(Z_ARRVAL_P(params), "notification", sizeof("notification"), (void**)&tmp)) {
		
		if (context->notifier) {
			php_stream_notification_free(context->notifier);
			context->notifier = NULL;
		}

		context->notifier = php_stream_notification_alloc();
		context->notifier->func = user_space_stream_notifier;
		context->notifier->ptr = *tmp;
		ZVAL_ADDREF(*tmp);
	}
	if (SUCCESS == zend_hash_find(Z_ARRVAL_P(params), "options", sizeof("options"), (void**)&tmp)) {
		parse_context_options(context, *tmp);
	}
	
	return ret;
}

/* given a zval which is either a stream or a context, return the underlying
 * stream_context.  If it is a stream that does not have a context assigned, it
 * will create and assign a context and return that.  */
static php_stream_context *decode_context_param(zval *contextresource TSRMLS_DC)
{
	php_stream_context *context = NULL;

	context = zend_fetch_resource(&contextresource TSRMLS_CC, -1, "Stream-Context", NULL, 1, le_stream_context);
	if (context == NULL) {
		php_stream *stream;

		php_stream_from_zval_no_verify(stream, &contextresource);

		if (stream) {
			context = stream->context;
			if (context == NULL) {
				context = stream->context = php_stream_context_alloc();
			}
		}
	}

	return context;
}
/* }}} */

/* {{{ proto array stream_context_get_options(resource context|resource stream)
   Retrieve options for a stream/wrapper/context */
PHP_FUNCTION(stream_context_get_options)
{
	zval *zcontext;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcontext) == FAILURE) {
		RETURN_FALSE;
	}
	context = decode_context_param(zcontext TSRMLS_CC);
	ZEND_VERIFY_RESOURCE(context);

	array_init(return_value);
	*return_value = *context->options;
	zval_copy_ctor(return_value);
		
}
/* }}} */

/* {{{ proto bool stream_context_set_option(resource context|resource stream, string wrappername, string optionname, mixed value)
   Set an option for a wrapper */
PHP_FUNCTION(stream_context_set_option)
{
	zval *options = NULL, *zcontext = NULL, *zvalue = NULL;
	php_stream_context *context;
	char *wrappername, *optionname;
	int wrapperlen, optionlen;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
				"rssz", &zcontext, &wrappername, &wrapperlen,
				&optionname, &optionlen, &zvalue) == FAILURE) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
					"ra", &zcontext, &options) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "called with wrong number or type of parameters; please RTM");
			RETURN_FALSE;
		}
	}

	/* figure out where the context is coming from exactly */
	context = decode_context_param(zcontext TSRMLS_CC);
	ZEND_VERIFY_RESOURCE(context);

	if (options) {
		/* handle the array syntax */
		RETVAL_BOOL(parse_context_options(context, options) == SUCCESS);
	} else {
		ZVAL_ADDREF(zvalue);
		php_stream_context_set_option(context, wrappername, optionname, zvalue);
		RETVAL_TRUE;
	}
}
/* }}} */

/* {{{ proto bool stream_context_set_params(resource context|resource stream, array options)
   Set parameters for a file context */
PHP_FUNCTION(stream_context_set_params)
{
	zval *params, *zcontext;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &zcontext, &params) == FAILURE) {
		RETURN_FALSE;
	}

	context = decode_context_param(zcontext TSRMLS_CC);
	ZEND_VERIFY_RESOURCE(context);

	RETVAL_BOOL(parse_context_params(context, params) == SUCCESS);
}
/* }}} */

/* {{{ proto resource stream_context_create([array options])
   Create a file context and optionally set parameters */
PHP_FUNCTION(stream_context_create)
{
	zval *params = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a", &params) == FAILURE) {
		RETURN_FALSE;
	}
	
	context = php_stream_context_alloc();
	
	if (params) {
		parse_context_options(context, params);
	}
	
	ZEND_REGISTER_RESOURCE(return_value, context, le_stream_context);
}
/* }}} */

/* {{{ streams filter functions */
static void apply_filter_to_stream(int append, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *zstream;
	php_stream *stream;
	char *filtername, *filterparams = NULL;
	int filternamelen, filterparamslen = 0, read_write = 0;
	php_stream_filter *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ls", &zstream,
				&filtername, &filternamelen, &read_write, &filterparams, &filterparamslen) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, &zstream);

	if ((read_write & PHP_STREAM_FILTER_ALL) == 0) {
		/* Chain not specified.
		 * Examine stream->mode to determine which filters are needed
		 * There's no harm in attaching a filter to an unused chain,
		 * but why waste the memory and clock cycles? 
		 */
		if (strchr(stream->mode, 'r') || strchr(stream->mode, '+')) {
			read_write |= PHP_STREAM_FILTER_READ;
		}
		if (strchr(stream->mode, 'w') || strchr(stream->mode, '+') || strchr(stream->mode, 'a')) {
			read_write |= PHP_STREAM_FILTER_WRITE;
		}
	}

	if (read_write & PHP_STREAM_FILTER_READ) {
		filter = php_stream_filter_create(filtername, filterparams, filterparamslen, php_stream_is_persistent(stream) TSRMLS_CC);
		if (filter == NULL) {
			RETURN_FALSE;
		}

		if (append) { 
			php_stream_filter_append(&stream->readfilters, filter);
		} else {
			php_stream_filter_prepend(&stream->readfilters, filter);
		}
	}

	if (read_write & PHP_STREAM_FILTER_WRITE) {
		filter = php_stream_filter_create(filtername, filterparams, filterparamslen, php_stream_is_persistent(stream) TSRMLS_CC);
		if (filter == NULL) {
			RETURN_FALSE;
		}

		if (append) { 
			php_stream_filter_append(&stream->writefilters, filter);
		} else {
			php_stream_filter_prepend(&stream->writefilters, filter);
		}
	}

	RETURN_TRUE;
}

/* {{{ proto bool stream_filter_prepend(resource stream, string filtername[, int read_write[, string filterparams]])
   Prepend a filter to a stream */
PHP_FUNCTION(stream_filter_prepend)
{
	apply_filter_to_stream(0, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool stream_filter_append(resource stream, string filtername[, int read_write[, string filterparams]])
   Append a filter to a stream */
PHP_FUNCTION(stream_filter_append)
{
	apply_filter_to_stream(1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string stream_get_line(resource stream, int maxlen, string ending)
   Read up to maxlen bytes from a stream or until the ending string is found */
PHP_FUNCTION(stream_get_line)
{
	char *str;
	int str_len;
	long max_length;
	zval *zstream;
	char *buf;
	size_t buf_size;
	php_stream *stream;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rls", &zstream, &max_length, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (max_length < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The maximum allowed length must be greater then or equal to zero.");
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, &zstream);

	if ((buf = php_stream_get_record(stream, max_length, &buf_size, str, str_len TSRMLS_CC))) {
		RETURN_STRINGL(buf, buf_size, 0);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto resource fopen(string filename, string mode [, bool use_include_path [, resource context]])
   Open a file or a URL and return a file pointer */
PHP_NAMED_FUNCTION(php_if_fopen)
{
	char *filename, *mode;
	int filename_len, mode_len;
	zend_bool use_include_path = 0;
	zval *zcontext = NULL;
	php_stream *stream;
	php_stream_context *context = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|br", &filename, &filename_len,
				&mode, &mode_len, &use_include_path, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}
	if (zcontext) {
		ZEND_FETCH_RESOURCE(context, php_stream_context*, &zcontext, -1, "stream-context", le_stream_context);
	}
	
	stream = php_stream_open_wrapper_ex(filename, mode,
				(use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS,
				NULL, context);

	if (stream == NULL) {
		RETURN_FALSE;
	}

	php_stream_to_zval(stream, return_value);

	return;
}
/* }}} */

/* {{{ proto bool fclose(resource fp)
   Close an open file pointer */
PHPAPI PHP_FUNCTION(fclose)
{
	zval **arg1;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);
	php_stream_close(stream);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto resource popen(string command, string mode)
   Execute a command and open either a read or a write pipe to it */

PHP_FUNCTION(popen)
{
	zval **arg1, **arg2;
	FILE *fp;
	char *p, *tmp = NULL;
	char *b, buf[1024];
	php_stream *stream;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
	p = estrndup(Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2));
	if (PG(safe_mode)){
		b = strchr(Z_STRVAL_PP(arg1), ' ');
		if (!b) {
			b = strrchr(Z_STRVAL_PP(arg1), '/');
		} else {
			char *c;
			c = Z_STRVAL_PP(arg1);
			while((*b != '/') && (b != c)) {
				b--;
			}
			if (b == c) {
				b = NULL;
			}
		}
		if (b) {
			snprintf(buf, sizeof(buf), "%s%s", PG(safe_mode_exec_dir), b);
		} else {
			snprintf(buf, sizeof(buf), "%s/%s", PG(safe_mode_exec_dir), Z_STRVAL_PP(arg1));
		}

		tmp = php_escape_shell_cmd(buf);
		fp = VCWD_POPEN(tmp, p);
		efree(tmp);

		if (!fp) {
			php_error_docref2(NULL TSRMLS_CC, buf, p, E_WARNING, "%s", strerror(errno));
			RETURN_FALSE;
		}
	} else {
		fp = VCWD_POPEN(Z_STRVAL_PP(arg1), p);
		if (!fp) {
			php_error_docref2(NULL TSRMLS_CC, Z_STRVAL_PP(arg1), p, E_WARNING, "%s", strerror(errno));
			efree(p);
			RETURN_FALSE;
		}
	}
	stream = php_stream_fopen_from_pipe(fp, p);

	if (stream == NULL)	{
		php_error_docref2(NULL TSRMLS_CC, Z_STRVAL_PP(arg1), p, E_WARNING, "%s", strerror(errno));
		RETVAL_FALSE;
	} else {
		php_stream_to_zval(stream, return_value);
	}

	efree(p);
}
/* }}} */

/* {{{ proto int pclose(resource fp)
   Close a file pointer opened by popen() */
PHP_FUNCTION(pclose)
{
	zval **arg1;
	php_stream *stream;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	RETURN_LONG(php_stream_close(stream));
}
/* }}} */

/* {{{ proto bool feof(resource fp)
   Test for end-of-file on a file pointer */
PHPAPI PHP_FUNCTION(feof)
{
	zval **arg1;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	if (php_stream_eof(stream)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool stream_set_blocking(resource socket, int mode)
   Set blocking/non-blocking mode on a socket or stream */
PHP_FUNCTION(stream_set_blocking)
{
	zval **arg1, **arg2;
	int block;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	convert_to_long_ex(arg2);
	block = Z_LVAL_PP(arg2);

	if (php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, block == 0 ? 0 : 1, NULL) == -1)
		RETURN_FALSE;
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool set_socket_blocking(resource socket, int mode)
   Set blocking/non-blocking mode on a socket */
PHP_FUNCTION(set_socket_blocking)
{
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "This function is deprecated, use stream_set_blocking() instead");
	PHP_FN(stream_set_blocking)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool stream_set_timeout(resource stream, int seconds, int microseconds)
   Set timeout on stream read to seconds + microseonds */
#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
PHP_FUNCTION(stream_set_timeout)
{
	zval **socket, **seconds, **microseconds;
	struct timeval t;
	php_stream *stream;

	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 3 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &socket, &seconds, &microseconds)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, socket);

	convert_to_long_ex(seconds);
	t.tv_sec = Z_LVAL_PP(seconds);

	if (ZEND_NUM_ARGS() == 3) {
		convert_to_long_ex(microseconds);
		t.tv_usec = Z_LVAL_PP(microseconds) % 1000000;
		t.tv_sec += Z_LVAL_PP(microseconds) / 1000000;
	}
	else
		t.tv_usec = 0;

	if (PHP_STREAM_OPTION_RETURN_OK == php_stream_set_option(stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &t)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
#endif /* HAVE_SYS_TIME_H || defined(PHP_WIN32) */
/* }}} */

/* {{{ proto string fgets(resource fp[, int length])
   Get a line from file pointer */
PHPAPI PHP_FUNCTION(fgets)
{
	zval **arg1, **arg2;
	int len = 1024;
	char *buf = NULL;
	int argc = ZEND_NUM_ARGS();
	size_t line_len = 0;
	php_stream *stream;

	if (argc<1 || argc>2 || zend_get_parameters_ex(argc, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	if (argc == 1) {
		/* ask streams to give us a buffer of an appropriate size */
		buf = php_stream_get_line(stream, NULL, 0, &line_len);
		if (buf == NULL)
			goto exit_failed;
	} else if (argc > 1) {
		convert_to_long_ex(arg2);
		len = Z_LVAL_PP(arg2);

		if (len < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter may not be negative");
			RETURN_FALSE;
		}

		buf = ecalloc(len + 1, sizeof(char));
		if (php_stream_get_line(stream, buf, len, &line_len) == NULL)
			goto exit_failed;
	}
	
	if (PG(magic_quotes_runtime)) {
		Z_STRVAL_P(return_value) = php_addslashes(buf, line_len, &Z_STRLEN_P(return_value), 1 TSRMLS_CC);
		Z_TYPE_P(return_value) = IS_STRING;
	} else {
		ZVAL_STRINGL(return_value, buf, line_len, 0);
		/* resize buffer if it's much larger than the result.
		 * Only needed if the user requested a buffer size. */
		if (argc > 1 && Z_STRLEN_P(return_value) < len / 2) {
			Z_STRVAL_P(return_value) = erealloc(buf, line_len + 1);
		}
	}
	return;

exit_failed:
	RETVAL_FALSE;
	if (buf)
		efree(buf);
}
/* }}} */

/* {{{ proto string fgetc(resource fp)
   Get a character from file pointer */
PHPAPI PHP_FUNCTION(fgetc)
{
	zval **arg1;
	char *buf;
	int result;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	buf = emalloc(2 * sizeof(char));

	result = php_stream_getc(stream);

	if (result == EOF) {
		efree(buf);
		RETVAL_FALSE;
	} else {
		buf[0] = result;
		buf[1] = '\0';

		RETURN_STRINGL(buf, 1, 0);
	}
}
/* }}} */

/* {{{ proto string fgetss(resource fp, int length [, string allowable_tags])
   Get a line from file pointer and strip HTML tags */
PHPAPI PHP_FUNCTION(fgetss)
{
	zval **fd, **bytes, **allow=NULL;
	int len;
	size_t actual_len, retval_len;
	char *buf;
	php_stream *stream;
	char *allowed_tags=NULL;
	int allowed_tags_len=0;

	switch(ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &fd, &bytes) == FAILURE) {
			RETURN_FALSE;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &fd, &bytes, &allow) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string_ex(allow);
		allowed_tags = Z_STRVAL_PP(allow);
		allowed_tags_len = Z_STRLEN_PP(allow);
		break;
	default:
		WRONG_PARAM_COUNT;
		/* NOTREACHED */
		break;
	}

	php_stream_from_zval(stream, fd);

	convert_to_long_ex(bytes);
	len = Z_LVAL_PP(bytes);
    if (len < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter may not be negative");
		RETURN_FALSE;
    }

	buf = emalloc(sizeof(char) * (len + 1));
	/*needed because recv doesnt set null char at end*/
	memset(buf, 0, len + 1);

	if (php_stream_get_line(stream, buf, len, &actual_len) == NULL)	{
		efree(buf);
		RETURN_FALSE;
	}

	/* strlen() can be used here since we are doing it on the return of an fgets() anyway */
	retval_len = php_strip_tags(buf, actual_len, &stream->fgetss_state, allowed_tags, allowed_tags_len);

	RETURN_STRINGL(buf, retval_len, 0);
}
/* }}} */

/* {{{ proto mixed fscanf(resource stream, string format [, string ...])
   Implements a mostly ANSI compatible fscanf() */
PHP_FUNCTION(fscanf)
{
	int  result;
	zval **file_handle, **format_string;
	size_t len;
	int type;
	char *buf;
	void *what;

	zval ***args;
	int argCount;

	argCount = ZEND_NUM_ARGS();
	if (argCount < 2) {
		WRONG_PARAM_COUNT;
	}
	args = (zval ***)emalloc(argCount * sizeof(zval **));
	if (zend_get_parameters_array_ex(argCount, args) == FAILURE) {
		efree( args );
		WRONG_PARAM_COUNT;
	}

	file_handle    = args[0];
	format_string  = args[1];

	what = zend_fetch_resource(file_handle TSRMLS_CC, -1, "File-Handle", &type, 2,
			php_file_le_stream(), php_file_le_pstream());

	/*
	 * we can't do a ZEND_VERIFY_RESOURCE(what), otherwise we end up
	 * with a leak if we have an invalid filehandle. This needs changing
	 * if the code behind ZEND_VERIFY_RESOURCE changed. - cc
	 */
	if (!what) {
		efree(args);
		RETURN_FALSE;
	}


	buf = php_stream_get_line((php_stream *) what, NULL, 0, &len);
	if (buf == NULL) {
		efree(args);
		RETURN_FALSE;
	}

	convert_to_string_ex(format_string);
	result = php_sscanf_internal(buf, Z_STRVAL_PP(format_string),
			argCount, args, 2, &return_value TSRMLS_CC);

	efree(args);
	efree(buf);

	if (SCAN_ERROR_WRONG_PARAM_COUNT == result) {
		WRONG_PARAM_COUNT;
	}

}
/* }}} */

/* {{{ proto int fwrite(resource fp, string str [, int length])
   Binary-safe file write */
PHPAPI PHP_FUNCTION(fwrite)
{
	zval **arg1, **arg2, **arg3=NULL;
	int ret;
	int num_bytes;
	char *buffer = NULL;
	php_stream *stream;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2)==FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string_ex(arg2);
		num_bytes = Z_STRLEN_PP(arg2);
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3)==FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string_ex(arg2);
		convert_to_long_ex(arg3);
		num_bytes = MIN(Z_LVAL_PP(arg3), Z_STRLEN_PP(arg2));
		break;
	default:
		WRONG_PARAM_COUNT;
		/* NOTREACHED */
		break;
	}

	php_stream_from_zval(stream, arg1);

	if (!arg3 && PG(magic_quotes_runtime)) {
		buffer = estrndup(Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2));
		php_stripslashes(buffer, &num_bytes TSRMLS_CC);
	}

	ret = php_stream_write(stream, buffer ? buffer : Z_STRVAL_PP(arg2), num_bytes);
	if (buffer) {
		efree(buffer);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto bool fflush(resource fp)
   Flushes output */
PHPAPI PHP_FUNCTION(fflush)
{
	zval **arg1;
	int ret;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	ret = php_stream_flush(stream);
	if (ret) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int stream_set_write_buffer(resource fp, int buffer)
   Set file write buffer */
PHP_FUNCTION(stream_set_write_buffer)
{
	zval **arg1, **arg2;
	int ret;
	size_t buff;
	php_stream *stream;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2)==FAILURE) {
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
		/* NOTREACHED */
		break;
	}
	
	php_stream_from_zval(stream, arg1);
	
	convert_to_long_ex(arg2);
	buff = Z_LVAL_PP(arg2);

	/* if buff is 0 then set to non-buffered */
	if (buff == 0) {
		ret = php_stream_set_option(stream, PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
	} else {
		ret = php_stream_set_option(stream, PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_FULL, &buff);
	}

	RETURN_LONG(ret == 0 ? 0 : EOF);
}
/* }}} */

/* {{{ proto bool rewind(resource fp)
   Rewind the position of a file pointer */
PHPAPI PHP_FUNCTION(rewind)
{
	zval **arg1;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	if (-1 == php_stream_rewind(stream)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftell(resource fp)
   Get file pointer's read/write position */
PHPAPI PHP_FUNCTION(ftell)
{
	zval **arg1;
	long ret;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	ret = php_stream_tell(stream);
	if (ret == -1)	{
		RETURN_FALSE;
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int fseek(resource fp, int offset [, int whence])
   Seek on a file pointer */
PHPAPI PHP_FUNCTION(fseek)
{
	zval **arg1, **arg2, **arg3;
	int argcount = ZEND_NUM_ARGS(), whence = SEEK_SET;
	php_stream *stream;

	if (argcount < 2 || argcount > 3 ||
	    zend_get_parameters_ex(argcount, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	convert_to_long_ex(arg2);
	if (argcount > 2) {
		convert_to_long_ex(arg3);
		whence = Z_LVAL_PP(arg3);
	}

	RETURN_LONG(php_stream_seek(stream, Z_LVAL_PP(arg2), whence));
}

/* }}} */

/* {{{ proto int mkdir(char *dir int mode)
*/

PHPAPI int php_mkdir(char *dir, long mode TSRMLS_DC)
{
	int ret;
	
	if (PG(safe_mode) && (!php_checkuid(dir, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		return -1;
	}

	if (php_check_open_basedir(dir TSRMLS_CC)) {
		return -1;
	}

	if ((ret = VCWD_MKDIR(dir, (mode_t)mode)) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, strerror(errno));
	}

	return ret;	
}
/* }}} */

/* {{{ proto bool mkdir(string pathname [, int mode [, bool recursive])
   Create a directory */
PHP_FUNCTION(mkdir)
{
	int dir_len, ret;
	long mode = 0777;
	char *dir;
	zend_bool recursive = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lb", &dir, &dir_len, &mode, &recursive) == FAILURE) {
		return;
	}

	if (!recursive) {
		ret = php_mkdir(dir, mode TSRMLS_CC);
	} else {
		/* we look for directory separator from the end of string, thus hopefuly reducing our work load */
		char *p, *e, *buf;
		struct stat sb;
		
		buf = estrndup(dir, dir_len);
		e = buf + dir_len;
		
		/* find a top level directory we need to create */
		while ((p = strrchr(buf, DEFAULT_SLASH))) {
			*p = '\0';
			if (VCWD_STAT(buf, &sb) == 0) {
				*p = DEFAULT_SLASH;
				break;
			}
		}
		if (p == buf) {
			ret = php_mkdir(dir, mode TSRMLS_CC);
		} else if (!(ret = php_mkdir(buf, mode TSRMLS_CC))) {
			if (!p) {
				p = buf;
			}
			/* create any needed directories if the creation of the 1st directory worked */
			while (++p != e) {
				if (*p == '\0' && *(p + 1) != '\0') {
					*p = DEFAULT_SLASH;
					if ((ret = VCWD_MKDIR(buf, (mode_t)mode)) < 0) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, strerror(errno));
						break;
					}
				}
			}
		}
		efree(buf);
	}

	if (ret < 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto bool rmdir(string dirname)
   Remove a directory */
PHP_FUNCTION(rmdir)
{
	zval **arg1;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);

	if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(arg1), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_PP(arg1) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ret = VCWD_RMDIR(Z_STRVAL_PP(arg1));
	if (ret < 0) {
		php_error_docref1(NULL TSRMLS_CC, Z_STRVAL_PP(arg1), E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int readfile(string filename [, int use_include_path])
   Output a file or a URL */
PHP_FUNCTION(readfile)
{
	zval **arg1, **arg2;
	int size=0;
	int use_include_path=0;
	php_stream *stream;
	
	/* check args */
	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg2);
		use_include_path = Z_LVAL_PP(arg2);
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);

	stream = php_stream_open_wrapper(Z_STRVAL_PP(arg1), "rb",
			(use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS,
			NULL);
	if (stream)	{
		size = php_stream_passthru(stream);
		php_stream_close(stream);
		RETURN_LONG(size);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int umask([int mask])
   Return or change the umask */
PHP_FUNCTION(umask)
{
	pval **arg1;
	int oldumask;
	int arg_count = ZEND_NUM_ARGS();

	oldumask = umask(077);

	if (arg_count == 0) {
		umask(oldumask);
	} else {
		if (arg_count > 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg1);
		umask(Z_LVAL_PP(arg1));
	}

	/* XXX we should maybe reset the umask after each request! */

	RETURN_LONG(oldumask);
}

/* }}} */

/* {{{ proto int fpassthru(resource fp)
   Output all remaining data from a file pointer */

PHPAPI PHP_FUNCTION(fpassthru)
{
	zval **arg1;
	int size;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	size = php_stream_passthru(stream);
	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto bool rename(string old_name, string new_name)
   Rename a file */
PHP_FUNCTION(rename)
{
	zval **old_arg, **new_arg;
	char *old_name, *new_name;
	int ret;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &old_arg, &new_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(old_arg);
	convert_to_string_ex(new_arg);

	old_name = Z_STRVAL_PP(old_arg);
	new_name = Z_STRVAL_PP(new_arg);

	if (PG(safe_mode) &&(!php_checkuid(old_name, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(old_name TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ret = VCWD_RENAME(old_name, new_name);

	if (ret == -1) {
		php_error_docref2(NULL TSRMLS_CC, old_name, new_name, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool unlink(string filename)
   Delete a file */
PHP_FUNCTION(unlink)
{
	zval **filename;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);

	if (PG(safe_mode) && !php_checkuid(Z_STRVAL_PP(filename), NULL, CHECKUID_CHECK_FILE_AND_DIR)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_PP(filename) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ret = VCWD_UNLINK(Z_STRVAL_PP(filename));
	if (ret == -1) {
		php_error_docref1(NULL TSRMLS_CC, Z_STRVAL_PP(filename), E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
	/* Clear stat cache */
	PHP_FN(clearstatcache)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftruncate(resource fp, int size)
   Truncate file to 'size' length */
PHP_NAMED_FUNCTION(php_if_ftruncate)
{
	zval **fp , **size;
	short int ret;
	int fd;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fp, &size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, fp);

	convert_to_long_ex(size);

	if (php_stream_is(stream, PHP_STREAM_IS_SOCKET))	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't truncate sockets!");
		RETURN_FALSE;
	}
	if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD, (void*)&fd, 1))	{
		ret = ftruncate(fd, Z_LVAL_PP(size));
		RETURN_LONG(ret + 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int fstat(resource fp)
   Stat() on a filehandle */
PHP_NAMED_FUNCTION(php_if_fstat)
{
	zval **fp;
	zval *stat_dev, *stat_ino, *stat_mode, *stat_nlink, *stat_uid, *stat_gid, *stat_rdev,
	 	*stat_size, *stat_atime, *stat_mtime, *stat_ctime, *stat_blksize, *stat_blocks;
	php_stream *stream;
	php_stream_statbuf stat_ssb;
	
	char *stat_sb_names[13]={"dev", "ino", "mode", "nlink", "uid", "gid", "rdev",
			      "size", "atime", "mtime", "ctime", "blksize", "blocks"};

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, fp);

	if (php_stream_stat(stream, &stat_ssb)) {
		RETURN_FALSE;
	}

	array_init(return_value);

	MAKE_LONG_ZVAL_INCREF(stat_dev, stat_ssb.sb.st_dev);
	MAKE_LONG_ZVAL_INCREF(stat_ino, stat_ssb.sb.st_ino);
	MAKE_LONG_ZVAL_INCREF(stat_mode, stat_ssb.sb.st_mode);
	MAKE_LONG_ZVAL_INCREF(stat_nlink, stat_ssb.sb.st_nlink);
	MAKE_LONG_ZVAL_INCREF(stat_uid, stat_ssb.sb.st_uid);
	MAKE_LONG_ZVAL_INCREF(stat_gid, stat_ssb.sb.st_gid);
#ifdef HAVE_ST_RDEV
	MAKE_LONG_ZVAL_INCREF(stat_rdev, stat_ssb.sb.st_rdev); 
#else
	MAKE_LONG_ZVAL_INCREF(stat_rdev, -1); 
#endif
	MAKE_LONG_ZVAL_INCREF(stat_size, stat_ssb.sb.st_size);
#if defined(NETWARE) && defined(CLIB_STAT_PATCH)
	MAKE_LONG_ZVAL_INCREF(stat_atime, stat_ssb.sb.st_atime.tv_sec);
	MAKE_LONG_ZVAL_INCREF(stat_mtime, stat_ssb.sb.st_mtime.tv_sec);
	MAKE_LONG_ZVAL_INCREF(stat_ctime, stat_ssb.sb.st_ctime.tv_sec);
#else
	MAKE_LONG_ZVAL_INCREF(stat_atime, stat_ssb.sb.st_atime);
	MAKE_LONG_ZVAL_INCREF(stat_mtime, stat_ssb.sb.st_mtime);
	MAKE_LONG_ZVAL_INCREF(stat_ctime, stat_ssb.sb.st_ctime);
#endif

#ifdef HAVE_ST_BLKSIZE
	MAKE_LONG_ZVAL_INCREF(stat_blksize, stat_ssb.sb.st_blksize); 
#else
	MAKE_LONG_ZVAL_INCREF(stat_blksize,-1);
#endif
#ifdef HAVE_ST_BLOCKS
	MAKE_LONG_ZVAL_INCREF(stat_blocks, stat_ssb.sb.st_blocks);
#else
	MAKE_LONG_ZVAL_INCREF(stat_blocks,-1);
#endif
	/* Store numeric indexes in propper order */
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_dev, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_ino, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_mode, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_nlink, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_uid, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_gid, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_rdev, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_size, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_atime, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_mtime, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_ctime, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_blksize, sizeof(zval *), NULL);
	zend_hash_next_index_insert(HASH_OF(return_value), (void *)&stat_blocks, sizeof(zval *), NULL);

	/* Store string indexes referencing the same zval*/
	zend_hash_update(HASH_OF(return_value), stat_sb_names[0], strlen(stat_sb_names[0])+1, (void *)&stat_dev, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[1], strlen(stat_sb_names[1])+1, (void *)&stat_ino, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[2], strlen(stat_sb_names[2])+1, (void *)&stat_mode, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[3], strlen(stat_sb_names[3])+1, (void *)&stat_nlink, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[4], strlen(stat_sb_names[4])+1, (void *)&stat_uid, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[5], strlen(stat_sb_names[5])+1, (void *)&stat_gid, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[6], strlen(stat_sb_names[6])+1, (void *)&stat_rdev, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[7], strlen(stat_sb_names[7])+1, (void *)&stat_size, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[8], strlen(stat_sb_names[8])+1, (void *)&stat_atime, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[9], strlen(stat_sb_names[9])+1, (void *)&stat_mtime, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[10], strlen(stat_sb_names[10])+1, (void *)&stat_ctime, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[11], strlen(stat_sb_names[11])+1, (void *)&stat_blksize, sizeof(zval *), NULL);
	zend_hash_update(HASH_OF(return_value), stat_sb_names[12], strlen(stat_sb_names[12])+1, (void *)&stat_blocks, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ proto bool copy(string source_file, string destination_file)
   Copy a file */
PHP_FUNCTION(copy)
{
	zval **source, **target;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &source, &target) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(source);
	convert_to_string_ex(target);

	if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(source), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_PP(source) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (php_copy_file(Z_STRVAL_PP(source), Z_STRVAL_PP(target) TSRMLS_CC)==SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_copy_file
 */
PHPAPI int php_copy_file(char *src, char *dest TSRMLS_DC)
{
	php_stream *srcstream = NULL, *deststream = NULL;
	int ret = FAILURE;

	srcstream = php_stream_open_wrapper(src, "rb", 
				STREAM_DISABLE_OPEN_BASEDIR | REPORT_ERRORS,
				NULL);
	
	if (!srcstream) {
		return ret;
	}

	deststream = php_stream_open_wrapper(dest, "wb", 
				ENFORCE_SAFE_MODE | REPORT_ERRORS,
				NULL);

	if (srcstream && deststream) {
		ret = php_stream_copy_to_stream(srcstream, deststream, PHP_STREAM_COPY_ALL) == 0 ? FAILURE : SUCCESS;
	}
	if (srcstream) {
		php_stream_close(srcstream);
	}
	if (deststream) {
		php_stream_close(deststream);
	}
	return ret;
}
/* }}} */

/* {{{ proto string fread(resource fp, int length)
   Binary-safe file read */
PHPAPI PHP_FUNCTION(fread)
{
	zval **arg1, **arg2;
	int len;
	php_stream *stream;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, arg1);

	convert_to_long_ex(arg2);
	len = Z_LVAL_PP(arg2);
	if (len < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter may not be negative");
		RETURN_FALSE;
	}

	Z_STRVAL_P(return_value) = emalloc(len + 1);
	Z_STRLEN_P(return_value) = php_stream_read(stream, Z_STRVAL_P(return_value), len);

	/* needed because recv/read/gzread doesnt put a null at the end*/
	Z_STRVAL_P(return_value)[Z_STRLEN_P(return_value)] = 0;

	if (PG(magic_quotes_runtime)) {
		Z_STRVAL_P(return_value) = php_addslashes(Z_STRVAL_P(return_value), 
				Z_STRLEN_P(return_value), &Z_STRLEN_P(return_value), 1 TSRMLS_CC);
	}
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ proto array fgetcsv(resource fp, int length [, string delimiter [, string enclosure]])
   Get line from file pointer and parse for CSV fields */
PHP_FUNCTION(fgetcsv)
{
	char *temp, *tptr, *bptr, *lineEnd;
	char delimiter = ',';	/* allow this to be set as parameter */
	char enclosure = '"';	/* allow this to be set as parameter */

	/* first section exactly as php_fgetss */

	zval **fd, **bytes, **p_delim, **p_enclosure;
	int len, temp_len;
	char *buf;
	php_stream *stream;

	switch(ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &fd, &bytes) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;

	case 3:
		if (zend_get_parameters_ex(3, &fd, &bytes, &p_delim) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(p_delim);
		/* Make sure that there is at least one character in string */
		if (Z_STRLEN_PP(p_delim) < 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Third parameter must be a character");
			return;
		}
		/* use first character from string */
		delimiter = Z_STRVAL_PP(p_delim)[0];
		break;

	case 4:
		if (zend_get_parameters_ex(4, &fd, &bytes, &p_delim, &p_enclosure) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(p_delim);
		/* Make sure that there is at least one character in string */
		if (Z_STRLEN_PP(p_delim) < 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Third parameter must be a character");
			return;
		}
		/* use first character from string */
		delimiter = Z_STRVAL_PP(p_delim)[0];

		convert_to_string_ex(p_enclosure);
		/* use first character from string */
		enclosure = Z_STRVAL_PP(p_enclosure)[0];
		
		break;

	default:
		WRONG_PARAM_COUNT;
		/* NOTREACHED */
		break;
	}

	php_stream_from_zval(stream, fd);

	convert_to_long_ex(bytes);
	len = Z_LVAL_PP(bytes);
	if (len < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter may not be negative");
		RETURN_FALSE;
	}

	buf = emalloc(len + 1);
	/* needed because recv/read/gzread doesnt set null char at end */
	memset(buf, 0, len + 1);

	if (php_stream_gets(stream, buf, len) == NULL) {
		efree(buf);
		RETURN_FALSE;
	}

	/* Now into new section that parses buf for delimiter/enclosure fields */

	/* Strip trailing space from buf, saving end of line in case required for enclosure field */

	lineEnd = emalloc(len + 1);
	bptr = buf;
	tptr = buf + strlen(buf) -1;
	while ( isspace((int)*(unsigned char *)tptr) && (*tptr!=delimiter) && (tptr > bptr) ) tptr--;
	tptr++;
	strcpy(lineEnd, tptr);

	/* add single space - makes it easier to parse trailing null field */
	*tptr++ = ' ';
	*tptr = 0;

	/* reserve workspace for building each individual field */

	temp_len = len;
	temp = emalloc(temp_len + 1);	/* unlikely but possible! */
	tptr = temp;

	/* Initialize return array */
	array_init(return_value);

	/* Main loop to read CSV fields */
	/* NB this routine will return a single null entry for a blank line */

	do {
		/* 1. Strip any leading space */
		while(isspace((int)*(unsigned char *)bptr) && (*bptr!=delimiter)) bptr++;
		/* 2. Read field, leaving bptr pointing at start of next field */
		if (enclosure && *bptr == enclosure) {
			bptr++;	/* move on to first character in field */

			/* 2A. handle enclosure delimited field */
			while (*bptr) {
				/* we need to determine if the enclosure is 'real' or is it escaped */
				if (*(bptr - 1) == '\\') {
					int escape_cnt = 0;
					char *bptr_p = bptr - 2;
				
					while (bptr_p > buf && *bptr_p == '\\') {
						escape_cnt++;
						bptr_p--;
					}
					if (!(escape_cnt % 2)) {
						goto normal_char;
						continue;
					}
				}
			
				if (*bptr == enclosure) {
					/* handle the enclosure */
					if ( *(bptr+1) == enclosure) {
					/* embedded enclosure */
						*tptr++ = *bptr; bptr +=2;
					} else {
					/* must be end of string - skip to start of next field or end */
						while ( (*bptr != delimiter) && *bptr ) bptr++;
						if (*bptr == delimiter) bptr++;
						*tptr=0;	/* terminate temporary string */
						break;	/* .. from handling this field - resumes at 3. */
					}
				} else {
normal_char:
				/* normal character */
					*tptr++ = *bptr++;

					if (*bptr == 0) {       /* embedded line end? */
						*(tptr-1)=0;            /* remove space character added on reading line */
						strcat(temp, lineEnd);   /* add the embedded line end to the field */

						/* read a new line from input, as at start of routine */
						memset(buf, 0, len+1);

						if (php_stream_gets(stream, buf, len) == NULL) {
							/* we've got an unterminated enclosure, assign all the data
							 * from the start of the enclosure to end of data to the last element
							 */
							if (temp_len > len) { 
								break;
							}
							
							efree(lineEnd); 
							efree(temp); 
							efree(buf);
							zval_dtor(return_value);
							RETURN_FALSE;
						}

						temp_len += len;
						temp = erealloc(temp, temp_len+1);
						bptr = buf;
						tptr = buf + strlen(buf) -1;
						while (isspace((int)*(unsigned char *)tptr) && (*tptr!=delimiter) && (tptr > bptr)) 
							tptr--;
						tptr++; 
						strcpy(lineEnd, tptr);
						*tptr++ = ' ';  
						*tptr = 0;

						tptr = temp;      /* reset temp pointer to end of field as read so far */
						while (*tptr) 
							tptr++;
					}
				}
			}
		} else {
			/* 2B. Handle non-enclosure field */
			while ((*bptr != delimiter) && *bptr) 
				*tptr++ = *bptr++;
			*tptr=0;	/* terminate temporary string */

			if (strlen(temp)) {
				tptr--;
				while (isspace((int)*(unsigned char *)tptr) && (*tptr!=delimiter)) 
					*tptr-- = 0;	/* strip any trailing spaces */
			}
			
			if (*bptr == delimiter) 
				bptr++;
		}

		/* 3. Now pass our field back to php */
		add_next_index_string(return_value, temp, 1);
		tptr = temp;
	} while (*bptr);

	efree(lineEnd);
	efree(temp);
	efree(buf);
}
/* }}} */


#if (!defined(PHP_WIN32) && !defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
/* {{{ proto string realpath(string path)
   Return the resolved path */
PHP_FUNCTION(realpath)
{
	zval **path;
	char resolved_path_buff[MAXPATHLEN];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &path) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(path);

	if (VCWD_REALPATH(Z_STRVAL_PP(path), resolved_path_buff)) {
		RETURN_STRING(resolved_path_buff, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* See http://www.w3.org/TR/html4/intro/sgmltut.html#h-3.2.2 */
#define PHP_META_HTML401_CHARS "-_.:"

/* {{{ php_next_meta_token
   Tokenizes an HTML file for get_meta_tags */
php_meta_tags_token php_next_meta_token(php_meta_tags_data *md TSRMLS_DC)
{
	int ch = 0, compliment;
	char buff[META_DEF_BUFSIZE + 1];

	memset((void *)buff, 0, META_DEF_BUFSIZE + 1);

	while (md->ulc || (!php_stream_eof(md->stream) && (ch = php_stream_getc(md->stream)))) {
		if(php_stream_eof(md->stream))
			break;

		if (md->ulc) {
			ch = md->lc;
			md->ulc = 0;
		}

        switch (ch) {
        case '<':
            return TOK_OPENTAG;
            break;
        case '>':
            return TOK_CLOSETAG;
            break;
        case '=':
            return TOK_EQUAL;
            break;
        case '/':
            return TOK_SLASH;
            break;
        case '\'':
        case '"':
            compliment = ch;
            md->token_len = 0;
            while (!php_stream_eof(md->stream) &&
				   (ch = php_stream_getc(md->stream)) &&
				   ch != compliment && ch != '<' && ch != '>') {

				buff[(md->token_len)++] = ch;

				if (md->token_len == META_DEF_BUFSIZE)
					break;
			}

			if (ch == '<' || ch == '>') {
				/* Was just an apostrohpe */
				md->ulc = 1;
				md->lc  = ch;
			}

			/* We don't need to alloc unless we are in a meta tag */
			if (md->in_meta) {
				md->token_data = (char *) emalloc(md->token_len + 1);
				memcpy(md->token_data, buff, md->token_len+1);
			}

			return TOK_STRING;
			break;
		case '\n':
		case '\r':
		case '\t':
			break;
		case ' ':
            return TOK_SPACE;
            break;
        default:
            if (isalnum(ch)) {
                md->token_len = 0;
                buff[(md->token_len)++] = ch;
				while (!php_stream_eof(md->stream) &&
					   (ch = php_stream_getc(md->stream)) &&
					   (isalnum(ch) || strchr(PHP_META_HTML401_CHARS, ch))) {

					buff[(md->token_len)++] = ch;

					if (md->token_len == META_DEF_BUFSIZE)
						break;
				}

				/* This is ugly, but we have to replace ungetc */
                if (!isalpha(ch) && ch != '-') {
					md->ulc = 1;
					md->lc  = ch;
				}

                md->token_data = (char *) emalloc(md->token_len + 1);
                memcpy(md->token_data, buff, md->token_len+1);

				return TOK_ID;
			} else {
				return TOK_OTHER;
			}
			break;
		}
	}

	return TOK_EOF;
}

/* }}} */

#ifdef HAVE_FNMATCH
/* {{{ proto bool fnmatch(string pattern, string filename [, int flags])
   Match filename against pattern */
PHP_FUNCTION(fnmatch)
{
	char *pattern = NULL;
	char *filename = NULL;
	int argc = ZEND_NUM_ARGS();
	int pattern_len;
	int filename_len;
	long flags=0;

	if (zend_parse_parameters(argc TSRMLS_CC, "ss|l", 
							  &pattern, &pattern_len, 
							  &filename, &filename_len, 
							  &flags) 
		== FAILURE) 
		return;
	
	RETURN_BOOL( ! fnmatch( pattern, filename, flags ));
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
