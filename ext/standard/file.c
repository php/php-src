/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Stig Bakken <ssb@php.net>                                   |
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
#include "php_smart_str.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef PHP_WIN32
#include <io.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#include "win32/winutil.h"
#elif defined(NETWARE)
#include <sys/param.h>
#include <sys/select.h>
#ifdef USE_WINSOCK
#include <novsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#else
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
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
#include "php_string.h"
#include "file.h"
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include "fsock.h"
#include "fopen_wrappers.h"
#include "streamsfuncs.h"
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

#include "scanf.h"
#include "zend_API.h"

#ifdef ZTS
int file_globals_id;
#else
php_file_globals file_globals;
#endif

#ifdef HAVE_FNMATCH
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fnmatch.h>
#endif

#ifdef HAVE_WCHAR_H
#include <wchar.h>
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#endif
/* }}} */

#define PHP_STREAM_TO_ZVAL(stream, arg) \
	php_stream_from_zval_no_verify(stream, arg); \
	if (stream == NULL) {	\
		RETURN_FALSE;	\
	}

/* {{{ ZTS-stuff / Globals / Prototypes */

/* sharing globals is *evil* */
static int le_stream_context = FAILURE;

PHPAPI int php_le_stream_context(void)
{
	return le_stream_context;
}
/* }}} */

/* {{{ Module-Stuff
*/
static ZEND_RSRC_DTOR_FUNC(file_context_dtor)
{
	php_stream_context *context = (php_stream_context*)rsrc->ptr;
	if (context->options) {
		zval_ptr_dtor(&context->options);
		context->options = NULL;
	}
	php_stream_context_free(context);
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
	STD_PHP_INI_ENTRY("default_socket_timeout", "60", PHP_INI_ALL, OnUpdateLong, default_socket_timeout, php_file_globals, file_globals)
	STD_PHP_INI_ENTRY("auto_detect_line_endings", "0", PHP_INI_ALL, OnUpdateLong, auto_detect_line_endings, php_file_globals, file_globals)
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
	REGISTER_LONG_CONSTANT("LOCK_SH", PHP_LOCK_SH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_EX", PHP_LOCK_EX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_UN", PHP_LOCK_UN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCK_NB", PHP_LOCK_NB, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_CONNECT", 		PHP_STREAM_NOTIFY_CONNECT,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_AUTH_REQUIRED",	PHP_STREAM_NOTIFY_AUTH_REQUIRED,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_AUTH_RESULT",		PHP_STREAM_NOTIFY_AUTH_RESULT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_MIME_TYPE_IS",	PHP_STREAM_NOTIFY_MIME_TYPE_IS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_FILE_SIZE_IS",	PHP_STREAM_NOTIFY_FILE_SIZE_IS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_REDIRECTED",		PHP_STREAM_NOTIFY_REDIRECTED,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_PROGRESS",		PHP_STREAM_NOTIFY_PROGRESS,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_FAILURE",			PHP_STREAM_NOTIFY_FAILURE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_COMPLETED",		PHP_STREAM_NOTIFY_COMPLETED,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_RESOLVE",			PHP_STREAM_NOTIFY_RESOLVE,			CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_SEVERITY_INFO",	PHP_STREAM_NOTIFY_SEVERITY_INFO, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_SEVERITY_WARN",	PHP_STREAM_NOTIFY_SEVERITY_WARN, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_NOTIFY_SEVERITY_ERR",	PHP_STREAM_NOTIFY_SEVERITY_ERR,  	CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_FILTER_READ",			PHP_STREAM_FILTER_READ,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_FILTER_WRITE",			PHP_STREAM_FILTER_WRITE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_FILTER_ALL",				PHP_STREAM_FILTER_ALL,				CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_CLIENT_PERSISTENT",		PHP_STREAM_CLIENT_PERSISTENT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CLIENT_ASYNC_CONNECT",	PHP_STREAM_CLIENT_ASYNC_CONNECT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CLIENT_CONNECT",			PHP_STREAM_CLIENT_CONNECT,	CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_CRYPTO_METHOD_SSLv2_CLIENT",		STREAM_CRYPTO_METHOD_SSLv2_CLIENT,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CRYPTO_METHOD_SSLv3_CLIENT",		STREAM_CRYPTO_METHOD_SSLv3_CLIENT,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CRYPTO_METHOD_SSLv23_CLIENT",	STREAM_CRYPTO_METHOD_SSLv23_CLIENT,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CRYPTO_METHOD_TLS_CLIENT",		STREAM_CRYPTO_METHOD_TLS_CLIENT,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CRYPTO_METHOD_SSLv2_SERVER",		STREAM_CRYPTO_METHOD_SSLv2_SERVER,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CRYPTO_METHOD_SSLv3_SERVER",		STREAM_CRYPTO_METHOD_SSLv3_SERVER,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CRYPTO_METHOD_SSLv23_SERVER",	STREAM_CRYPTO_METHOD_SSLv23_SERVER,	CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_CRYPTO_METHOD_TLS_SERVER",		STREAM_CRYPTO_METHOD_TLS_SERVER,	CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_SHUT_RD",	STREAM_SHUT_RD,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_SHUT_WR",	STREAM_SHUT_WR,		CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_SHUT_RDWR",	STREAM_SHUT_RDWR,	CONST_CS|CONST_PERSISTENT);

#ifdef PF_INET
	REGISTER_LONG_CONSTANT("STREAM_PF_INET", PF_INET, CONST_CS|CONST_PERSISTENT);
#elif defined(AF_INET)
	REGISTER_LONG_CONSTANT("STREAM_PF_INET", AF_INET, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef PF_INET6
	REGISTER_LONG_CONSTANT("STREAM_PF_INET6", PF_INET6, CONST_CS|CONST_PERSISTENT);
#elif defined(AF_INET6)
	REGISTER_LONG_CONSTANT("STREAM_PF_INET6", AF_INET6, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef PF_UNIX
	REGISTER_LONG_CONSTANT("STREAM_PF_UNIX", PF_UNIX, CONST_CS|CONST_PERSISTENT);
#elif defined(AF_UNIX)
	REGISTER_LONG_CONSTANT("STREAM_PF_UNIX", AF_UNIX, CONST_CS|CONST_PERSISTENT);
#endif

#ifdef IPPROTO_IP
	/* most people will use this one when calling socket() or socketpair() */
	REGISTER_LONG_CONSTANT("STREAM_IPPROTO_IP", IPPROTO_IP, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef IPPROTO_TCP
	REGISTER_LONG_CONSTANT("STREAM_IPPROTO_TCP", IPPROTO_TCP, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef IPPROTO_UDP
	REGISTER_LONG_CONSTANT("STREAM_IPPROTO_UDP", IPPROTO_UDP, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef IPPROTO_ICMP
	REGISTER_LONG_CONSTANT("STREAM_IPPROTO_ICMP", IPPROTO_ICMP, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef IPPROTO_RAW
	REGISTER_LONG_CONSTANT("STREAM_IPPROTO_RAW", IPPROTO_RAW, CONST_CS|CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("STREAM_SOCK_STREAM", SOCK_STREAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_SOCK_DGRAM", SOCK_DGRAM, CONST_CS|CONST_PERSISTENT);
#ifdef SOCK_RAW
	REGISTER_LONG_CONSTANT("STREAM_SOCK_RAW", SOCK_RAW, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef SOCK_SEQPACKET
	REGISTER_LONG_CONSTANT("STREAM_SOCK_SEQPACKET", SOCK_SEQPACKET, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef SOCK_RDM
	REGISTER_LONG_CONSTANT("STREAM_SOCK_RDM", SOCK_RDM, CONST_CS|CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("STREAM_PEEK", STREAM_PEEK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_OOB",  STREAM_OOB, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("STREAM_SERVER_BIND",			STREAM_XPORT_BIND,					CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_SERVER_LISTEN",			STREAM_XPORT_LISTEN,				CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILE_USE_INCLUDE_PATH",			PHP_FILE_USE_INCLUDE_PATH,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILE_IGNORE_NEW_LINES",			PHP_FILE_IGNORE_NEW_LINES,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILE_SKIP_EMPTY_LINES",			PHP_FILE_SKIP_EMPTY_LINES,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILE_APPEND", 					PHP_FILE_APPEND,					CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILE_NO_DEFAULT_CONTEXT",		PHP_FILE_NO_DEFAULT_CONTEXT,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILE_TEXT", 					PHP_FILE_TEXT,						CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILE_BINARY", 					PHP_FILE_BINARY,					CONST_CS | CONST_PERSISTENT);

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

PHP_MSHUTDOWN_FUNCTION(file) /* {{{ */
{
#ifndef ZTS
	file_globals_dtor(&file_globals TSRMLS_CC);
#endif
	return SUCCESS;
}
/* }}} */

static int flock_values[] = { LOCK_SH, LOCK_EX, LOCK_UN };

/* {{{ proto bool flock(resource fp, int operation [, int &wouldblock]) U
   Portable file locking */
PHP_FUNCTION(flock)
{
	zval *arg1, *arg3 = NULL;
	int act;
	php_stream *stream;
	long operation = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|z", &arg1, &operation, &arg3) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	act = operation & 3;
	if (act < 1 || act > 3) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal operation argument");
		RETURN_FALSE;
	}

	if (arg3 && PZVAL_IS_REF(arg3)) {
		convert_to_long_ex(&arg3);
		Z_LVAL_P(arg3) = 0;
	}

	/* flock_values contains all possible actions if (operation & 4) we won't block on the lock */
	act = flock_values[act - 1] | (operation & PHP_LOCK_NB ? LOCK_NB : 0);
	if (php_stream_lock(stream, act)) {
		if (operation && errno == EWOULDBLOCK && arg3 && PZVAL_IS_REF(arg3)) {
			Z_LVAL_P(arg3) = 1;
		}
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#define PHP_META_UNSAFE ".\\+*?[^]$() "

/* {{{ proto array get_meta_tags(string filename [, bool use_include_path]) U
   Extracts all meta tag content attributes from a file and returns an array */
PHP_FUNCTION(get_meta_tags)
{
	zval **ppfilename;
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
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|b", &ppfilename, &use_include_path) == FAILURE ||
		php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, FG(default_context)) == FAILURE) {
		return;
	}

	md.stream = php_stream_open_wrapper(filename, "rt",
			(use_include_path ? USE_PATH : 0) | REPORT_ERRORS,
			NULL);
	if (!md.stream)	{
		RETURN_FALSE;
	}

	if (md.stream->readbuf_type == IS_UNICODE) {
		/* Either stream auto-applied encoding (which http:// wrapper does do)
		 * Or the streams layer unicodified it for us */
		zval *filterparams;
		php_stream_filter *filter;

		/* Be lazy and convert contents to utf8 again
		 * This could be made more efficient by detecting if
		 * it's being upconverted from utf8 and cancelling all conversion
		 * rather than reconverting, but this is a silly function anyway */

		MAKE_STD_ZVAL(filterparams);
		array_init(filterparams);
		add_ascii_assoc_long(filterparams, "error_mode", UG(from_error_mode));
		add_ascii_assoc_unicode(filterparams, "subst_char", UG(from_subst_char), 1);
		filter = php_stream_filter_create("unicode.to.utf8", filterparams, 0 TSRMLS_CC);
		zval_ptr_dtor(&filterparams);

		if (!filter) {
			php_stream_close(md.stream);
			RETURN_FALSE;
		}
		php_stream_filter_append(&md.stream->readfilters, filter);
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
					STR_FREE(name);
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
					STR_FREE(value);
					/* Get the CONTENT attr (Single word attr, non-quoted) */
					value = estrndup(md.token_data, md.token_len);

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
				STR_FREE(name);
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
				STR_FREE(value);
				/* Get the CONTENT attr (Single word attr, non-quoted) */
				value = estrndup(md.token_data, md.token_len);

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
					add_assoc_utf8_string(return_value, name, value, 1);
				} else {
					add_assoc_utf8_string(return_value, name, "", 1);
				}

				efree(name);
				STR_FREE(value);
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

	STR_FREE(value);
	STR_FREE(name);
	php_stream_close(md.stream);
}
/* }}} */

/* {{{ proto string file_get_contents(string filename [, long flags [, resource context [, long offset [, long maxlen]]]]) U
   Read the entire file into a string */
PHP_FUNCTION(file_get_contents)
{
	zval **ppfilename;
	char *filename;
	int filename_len;
	void *contents = NULL;
	long flags = 0;
	php_stream *stream;
	int len;
	long offset = -1;
	long maxlen = PHP_STREAM_COPY_ALL, real_maxlen;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;

	/* Parse arguments */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|lr!ll", &ppfilename, &flags, &zcontext, &offset, &maxlen) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 5 && maxlen < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "length must be greater than or equal to zero");
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);
	if (php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, context) == FAILURE) {
		RETURN_FALSE;
	}

	stream = php_stream_open_wrapper_ex(filename, (flags & PHP_FILE_TEXT) ? "rt" : "rb",
				((flags & PHP_FILE_USE_INCLUDE_PATH) ? USE_PATH : 0) | REPORT_ERRORS,
				NULL, context);
	if (!stream) {
		RETURN_FALSE;
	}

	if (offset > 0 && php_stream_seek(stream, offset, SEEK_SET) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to seek to position %ld in the stream", offset);
		php_stream_close(stream);
		RETURN_FALSE;
	}

	if (maxlen <= 0 || stream->readbuf_type == IS_STRING) {
		real_maxlen = maxlen;
	} else {
		/* Allows worst case scenario of each input char being turned into two UChars
		 * UTODO: Have this take converter into account, since many never generate surrogate pairs */
		real_maxlen = maxlen * 2;
	}

	/* uses mmap if possible */
	len = php_stream_copy_to_mem_ex(stream, stream->readbuf_type, &contents, real_maxlen, maxlen, 0);

	if (stream->readbuf_type == IS_STRING) {
		if (len > 0) {
			RETVAL_STRINGL(contents, len, 0);
		} else {
			if (contents) {
				efree(contents);
			}
			RETVAL_EMPTY_STRING();
		}
	} else {
		if (len > 0) {
			RETVAL_UNICODEL(contents, len, 0);
		} else {
			if (contents) {
				efree(contents);
			}
			RETVAL_EMPTY_UNICODE();
		}
	}

	php_stream_close(stream);
}
/* }}} */

/* {{{ proto int file_put_contents(string file, mixed data [, int flags [, resource context]]) U
   Write/Create a file with contents data and return the number of bytes written */
PHP_FUNCTION(file_put_contents)
{
	int argc = ZEND_NUM_ARGS();
	php_stream *stream;
	zval **ppfilename;
	char *filename;
	int filename_len;
	zval *data;
	int numchars = 0;
	long flags = ((argc < 3) && UG(unicode)) ? PHP_FILE_TEXT : 0;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;
	char mode[3] = { 'w', 0, 0 };
	php_stream *srcstream = NULL;

	if (zend_parse_parameters(argc TSRMLS_CC, "Zz/|lr!", &ppfilename, &data, &flags, &zcontext) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(data) == IS_RESOURCE) {
		php_stream_from_zval(srcstream, &data);
	}

	context = php_stream_context_from_zval(zcontext, flags & PHP_FILE_NO_DEFAULT_CONTEXT);
	if (php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, context) == FAILURE) {
		RETURN_FALSE;
	}

	if (flags & PHP_FILE_APPEND) {
		mode[0] = 'a';
	}
	if (flags & PHP_FILE_BINARY || (Z_TYPE_P(data) == IS_STRING)) {
		mode[1] = 'b';
	} else if (flags & PHP_FILE_TEXT) {
		mode[1] = 't';
	}

	stream = php_stream_open_wrapper_ex(filename, mode,
			((flags & PHP_FILE_USE_INCLUDE_PATH) ? USE_PATH : 0) | REPORT_ERRORS, NULL, context);
	if (stream == NULL) {
		RETURN_FALSE;
	}

	if (flags & LOCK_EX && (!php_stream_supports_lock(stream) || php_stream_lock(stream, LOCK_EX))) {
		php_stream_close(stream);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Exclusive locks are not supported for this stream");
		RETURN_FALSE;
	}

	switch (Z_TYPE_P(data)) {
		case IS_RESOURCE:
			numchars = php_stream_copy_to_stream(srcstream, stream, PHP_STREAM_COPY_ALL);
			break;
		case IS_ARRAY:
			if (zend_hash_num_elements(Z_ARRVAL_P(data))) {
				zval **tmp;
				HashPosition pos;

				zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(data), &pos);
				while (zend_hash_get_current_data_ex(Z_ARRVAL_P(data), (void **) &tmp, &pos) == SUCCESS) {
					if (Z_TYPE_PP(tmp) == IS_UNICODE) {
						int ustrlen = u_countChar32(Z_USTRVAL_PP(tmp), Z_USTRLEN_PP(tmp));
						int wrote_u16 = php_stream_write_unicode(stream, Z_USTRVAL_PP(tmp), Z_USTRLEN_PP(tmp));
						if (wrote_u16 < 0) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to write %d characters to %s", ustrlen, filename);
							numchars = -1;
							break;
						} else if (wrote_u16 != Z_USTRLEN_PP(tmp)) {
							int numchars = u_countChar32(Z_USTRVAL_PP(tmp), wrote_u16);

							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only %d of %d characters written, possibly out of free disk space", numchars, ustrlen);
							numchars = -1;
							break;
						}
						numchars += ustrlen;
					} else { /* non-unicode */
						int free_val = 0;
						zval strval = **tmp;
						int wrote_bytes;

						if (Z_TYPE(strval) != IS_STRING) {
							zval_copy_ctor(&strval);
							convert_to_string(&strval);
							free_val = 1;
						}
						if (Z_STRLEN(strval)) {
							numchars += Z_STRLEN(strval);
							wrote_bytes = php_stream_write(stream, Z_STRVAL(strval), Z_STRLEN(strval));
							if (wrote_bytes < 0) {
								php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to write %d bytes to %s", Z_STRLEN(strval), filename);
								numchars = -1;
								break;
							}
							if (wrote_bytes != Z_STRLEN(strval)) {
								php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only %d of %d bytes written, possibly out of free disk space", wrote_bytes, Z_STRLEN(strval));
								numchars = -1;
								break;
							}
						}
						if (free_val) {
							zval_dtor(&strval);
						}
					}
					zend_hash_move_forward_ex(Z_ARRVAL_P(data), &pos);
				}
			}
			break;
		case IS_OBJECT:
			/* TODO */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "2nd parameter must be non-object (for now)");
			numchars = -1;
			break;
		case IS_UNICODE:
			if (Z_USTRLEN_P(data)) {
				int ustrlen = u_countChar32(Z_USTRVAL_P(data), Z_USTRLEN_P(data));
				int wrote_u16 = php_stream_write_unicode(stream, Z_USTRVAL_P(data), Z_USTRLEN_P(data));

				numchars = ustrlen;
				if (wrote_u16 < 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to write %d characters to %s", ustrlen, filename);
					numchars = -1;
				} else if (wrote_u16 != Z_USTRLEN_P(data)) {
					int written_numchars = u_countChar32(Z_USTRVAL_P(data), wrote_u16);

					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only %d of %d characters written, possibly out of free disk space", written_numchars, ustrlen);
					numchars = -1;
				}
			}
			break;
		case IS_NULL:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_BOOL:
		case IS_CONSTANT:
		case IS_STRING:
		default:
			if (Z_TYPE_P(data) != IS_STRING) {
				convert_to_string_ex(&data);
			}
			if (Z_STRLEN_P(data)) {
				numchars = php_stream_write(stream, Z_STRVAL_P(data), Z_STRLEN_P(data));
				if (numchars != Z_STRLEN_P(data)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only %d of %d bytes written, possibly out of free disk space", numchars, Z_STRLEN_P(data));
					numchars = -1;
				}
			}
			break;
	}
	php_stream_close(stream);

	if (numchars < 0) {
		RETURN_FALSE;
	}

	RETURN_LONG(numchars);
}
/* }}} */

#define PHP_FILE_BUF_SIZE	80

/* {{{ proto array file(string filename [, int flags[, resource context]]) U
   Read entire file into an array */
/* UTODO: Accept unicode contents */
PHP_FUNCTION(file)
{
	zval **ppfilename;
	char *filename;
	int filename_len;
	char *target_buf=NULL;
	register int i = 0;
	int target_len;
	long flags = 0;
	zend_bool use_include_path;
	zend_bool include_new_line;
	zend_bool skip_blank_lines;
	zend_bool text_mode;
	php_stream *stream;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;

	/* Parse arguments */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|lr!", &ppfilename, &flags, &zcontext) == FAILURE) {
		return;
	}
	if (flags < 0 || flags > (PHP_FILE_USE_INCLUDE_PATH | PHP_FILE_IGNORE_NEW_LINES | PHP_FILE_SKIP_EMPTY_LINES | PHP_FILE_NO_DEFAULT_CONTEXT | PHP_FILE_TEXT)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%ld' flag is not supported", flags);
		RETURN_FALSE;
	}

	use_include_path = flags & PHP_FILE_USE_INCLUDE_PATH;
	include_new_line = !(flags & PHP_FILE_IGNORE_NEW_LINES);
	skip_blank_lines = flags & PHP_FILE_SKIP_EMPTY_LINES;
	text_mode = flags & PHP_FILE_TEXT;

	context = php_stream_context_from_zval(zcontext, flags & PHP_FILE_NO_DEFAULT_CONTEXT);
	if (php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, context) == FAILURE) {
		RETURN_FALSE;
	}

	stream = php_stream_open_wrapper_ex(filename, text_mode ? "rt" : "rb", (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, context);
	if (!stream) {
		RETURN_FALSE;
	}

	/* Initialize return array */
	array_init(return_value);

	target_len = php_stream_copy_to_mem_ex(stream, stream->readbuf_type, (void**)&target_buf, PHP_STREAM_COPY_ALL, -1, 0);

	if (!target_len) {
		/* Empty file, do nothing and return an empty array */
	} else if (stream->readbuf_type == IS_UNICODE) {
		UChar *s = (UChar*)target_buf, *p;
		UChar *e = s + target_len, eol_marker = '\n';

		if (!(p = php_stream_locate_eol(stream, ZSTR(target_buf), target_len TSRMLS_CC))) {
			p = e;
			goto uparse_eol;
		}

		if (stream->flags & PHP_STREAM_FLAG_EOL_MAC) {
			eol_marker = '\r';
		}

		/* for performance reasons the code is quadruplicated, so that the if (include_new_line/unicode
		 * will not need to be done for every single line in the file. */
		if (include_new_line) {
			do {
				p++;
uparse_eol:
				add_index_unicodel(return_value, i++, eustrndup(s, p-s), p-s, 0);
				s = p;
			} while ((p = u_memchr(p, eol_marker, (e-p))));
		} else {
			do {
				if (skip_blank_lines && !(p-s)) {
					s = ++p;
					continue;
				}
				add_index_unicodel(return_value, i++, eustrndup(s, p-s), p-s, 0);
				s = ++p;
			} while ((p = u_memchr(p, eol_marker, (e-p))));
		}

		/* handle any left overs of files without new lines */
		if (s != e) {
			p = e;
			goto uparse_eol;
		}

	} else { /* !IS_UNICODE */
		char *s = target_buf, *p;
		char *e = target_buf + target_len, eol_marker = '\n';

		if (!(p = php_stream_locate_eol(stream, ZSTR(target_buf), target_len TSRMLS_CC))) {
			p = e;
			goto parse_eol;
		}

		if (stream->flags & PHP_STREAM_FLAG_EOL_MAC) {
			eol_marker = '\r';
		}

		if (include_new_line) {
			do {
				p++;
parse_eol:
				add_index_stringl(return_value, i++, estrndup(s, p-s), p-s, 0);
				s = p;
			} while ((p = memchr(p, eol_marker, (e-p))));
		} else {
			do {
				if (skip_blank_lines && !(p-s)) {
					s = ++p;
					continue;
				}
				add_index_stringl(return_value, i++, estrndup(s, p-s), p-s, 0);
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

/* {{{ proto string tempnam(string dir, string prefix) U
   Create a unique filename in a directory */
PHP_FUNCTION(tempnam)
{
	zval **ppdir, **ppprefix;
	char *dir, *prefix;
	int dir_len, prefix_len, p_len;
	char *opened_path;
	char *p;
	int fd;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &ppdir, &ppprefix) == FAILURE) {
		return;
	}

	if (php_stream_path_param_encode(ppdir, &dir, &dir_len, REPORT_ERRORS, FG(default_context)) == FAILURE ||
		php_stream_path_param_encode(ppprefix, &prefix, &prefix_len, REPORT_ERRORS, FG(default_context)) == FAILURE) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(dir TSRMLS_CC)) {
		RETURN_FALSE;
	}

	php_basename(prefix, prefix_len, NULL, 0, &p, &p_len TSRMLS_CC);
	if (p_len > 64) {
		p[63] = '\0';
	}

	if ((fd = php_open_temporary_fd(dir, p, &opened_path TSRMLS_CC)) >= 0) {
		close(fd);
		if (UG(unicode)) {
			UChar *utmpnam;
			int utmpnam_len;

			if (SUCCESS == php_stream_path_decode(NULL, &utmpnam, &utmpnam_len, opened_path, strlen(opened_path), REPORT_ERRORS, FG(default_context))) {
				RETVAL_UNICODEL(utmpnam, utmpnam_len, 0);
			}
			efree(opened_path);
		} else {
			RETVAL_STRING(opened_path, 0);
		}
	}
	efree(p);
}
/* }}} */

/* {{{ proto resource tmpfile(void) U
   Create a temporary file that will be deleted automatically after use */
PHP_NAMED_FUNCTION(php_if_tmpfile)
{
	php_stream *stream;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	stream = php_stream_fopen_tmpfile();

	if (stream) {
		php_stream_to_zval(stream, return_value);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource fopen(string filename, string mode [, bool use_include_path [, resource context]]) U
   Open a file or a URL and return a file pointer */
PHP_NAMED_FUNCTION(php_if_fopen)
{
	zval **ppfilename;
	char *filename, *mode;
	int filename_len, mode_len;
	zend_bool use_include_path = 0;
	zval *zcontext = NULL;
	php_stream *stream;
	php_stream_context *context = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zs|br", &ppfilename, &mode, &mode_len, &use_include_path, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);

	if (FAILURE == php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, context)) {
		RETURN_FALSE;
	}

	stream = php_stream_open_wrapper_ex(filename, mode, (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, context);
	if (stream == NULL) {
		RETURN_FALSE;
	}
	
	php_stream_to_zval(stream, return_value);
}
/* }}} */

/* {{{ proto bool fclose(resource fp) U
   Close an open file pointer */
PHPAPI PHP_FUNCTION(fclose)
{
	zval *arg1;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);
	
	if ((stream->flags & PHP_STREAM_FLAG_NO_FCLOSE) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%d is not a valid stream resource", stream->rsrc_id);
		RETURN_FALSE;
	}
	
	if (!stream->is_persistent) {
		zend_list_delete(stream->rsrc_id);
	} else {
		php_stream_pclose(stream);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource popen(string command, string mode) U
   Execute a command and open either a read or a write pipe to it */
PHP_FUNCTION(popen)
{
	zval **ppcommand;
	char *command, *mode;
	int command_len, mode_len;
	FILE *fp;
	char *posix_mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zs", &ppcommand, &mode, &mode_len) == FAILURE) {
		return;
	}

	if (php_stream_path_param_encode(ppcommand, &command, &command_len, REPORT_ERRORS, FG(default_context)) == FAILURE) {
		RETURN_FALSE;
	}

	posix_mode = estrndup(mode, mode_len);
#ifndef PHP_WIN32
	{
		char *z = memchr(posix_mode, 'b', mode_len);
		if (z) {
			memmove(z, z + 1, mode_len - (z - posix_mode));
		}
	}
#endif
	fp = VCWD_POPEN(command, posix_mode);
	if (!fp) {
		php_error_docref2(NULL TSRMLS_CC, command, mode, E_WARNING, "%s", strerror(errno));
		RETVAL_FALSE;
	} else {
		php_stream *stream = php_stream_fopen_from_pipe(fp, mode);

		if (stream == NULL)	{
			php_error_docref2(NULL TSRMLS_CC, command, mode, E_WARNING, "%s", strerror(errno));
			RETVAL_FALSE;
		} else {
			php_stream_to_zval(stream, return_value);
		}
	}

	efree(posix_mode);
}
/* }}} */

/* {{{ proto int pclose(resource fp) U
   Close a file pointer opened by popen() */
PHP_FUNCTION(pclose)
{
	zval *arg1;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	zend_list_delete(stream->rsrc_id);
	RETURN_LONG(FG(pclose_ret));
}
/* }}} */

/* {{{ proto bool feof(resource fp) U
   Test for end-of-file on a file pointer */
PHPAPI PHP_FUNCTION(feof)
{
	zval *arg1;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	if (php_stream_eof(stream)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string fgets(resource fp[, int lengthish]) U
   Get a line from file pointer */
PHPAPI PHP_FUNCTION(fgets)
{
	php_stream *stream;
	zval *zstream;
	int argc = ZEND_NUM_ARGS();
	long length = -1;
	long len;
	zstr buf, line;
	size_t retlen = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "r|l", &zstream, &length) == FAILURE) {
		return;
	}

	php_stream_from_zval(stream, &zstream);

	if (argc == 2 && length <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than 0");
		RETURN_FALSE;
	}

	if (length == 1) {
		/* For BC reasons, fgets() should only return length-1 bytes. */
		RETURN_FALSE;
	} else if (length > 1) {
		len = length;
		buf.v = ecalloc(len, (stream->readbuf_type == IS_UNICODE) ? sizeof(UChar) : sizeof(char));
		length--;
	} else {
		buf.v = NULL;
		len = -1;
	}

	line.v = php_stream_get_line_ex(stream, stream->readbuf_type, buf, len, length, &retlen);
	if (!line.v) {
		if (buf.v) {
			efree(buf.v);
		}
		RETURN_FALSE;
	}

	if (stream->readbuf_type == IS_UNICODE) {
		RETURN_UNICODEL(line.u, retlen, 0);
	} else { /* IS_STRING */
		RETURN_STRINGL(line.s, retlen, 0);
	}
}
/* }}} */

/* {{{ proto string fgetc(resource fp) U
   Get a character from file pointer */
PHPAPI PHP_FUNCTION(fgetc)
{
	zval *arg1;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	if (stream->readbuf_type == IS_UNICODE) {
		int buflen = 1;
		UChar *buf = php_stream_read_unicode_chars(stream, &buflen);

		if (!buf || !buflen) {
			if (buf) {
				efree(buf);
			}
			RETURN_FALSE;
		}
		RETURN_UNICODEL(buf, buflen, 0);
	} else { /* IS_STRING */
		char buf[2];

		if ((buf[0] = php_stream_getc(stream)) == EOF) {
			RETURN_FALSE;
		}
		buf[1] = 0;
		RETURN_STRINGL(buf, 1, 1);
	}
}
/* }}} */

/* {{{ proto string fgetss(resource fp [, int lengthish [, string allowable_tags]]) U
   Get a line from file pointer and strip HTML tags */
PHPAPI PHP_FUNCTION(fgetss)
{
	zval *zstream;
	php_stream *stream;
	long length = 0;
	long len;
	zval **allow = NULL;
	size_t retlen = 0;
	zstr buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|lZ", &zstream, &length, &allow) == FAILURE) {
		return;
	}

	php_stream_from_zval(stream, &zstream);

	if (ZEND_NUM_ARGS() >= 2 && length <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than 0");
		RETURN_FALSE;
	}

	if (length == 1) {
		/* For BC reasons, fgetss() should only return length-1 bytes. */
		RETURN_FALSE;
	} else if (length > 1) {
		len = length;
		buf.v = ecalloc(len, (stream->readbuf_type == IS_UNICODE) ? sizeof(UChar) : sizeof(char));
		length--;
	} else {
		buf.v = NULL;
		len = -1;
	}

	if (stream->readbuf_type == IS_UNICODE) {
		UChar *line;
		UChar *allowed = NULL;
		int allowed_len = 0;

		line = php_stream_get_line_ex(stream, IS_UNICODE, buf, len, length, &retlen);
		if (!line) {
			if (buf.v) {
				efree(buf.v);
			}
			RETURN_FALSE;
		}

		if (allow) {
			convert_to_unicode_ex(allow);
			allowed = Z_USTRVAL_PP(allow);
			allowed_len = Z_USTRLEN_PP(allow);
		}
		retlen = php_u_strip_tags(line, retlen, &stream->fgetss_state, allowed, allowed_len TSRMLS_CC);

		RETURN_UNICODEL(line, retlen, 0);
	} else { /* IS_STRING */
		char *line;
		char *allowed = NULL;
		int allowed_len = 0;

		line = php_stream_get_line_ex(stream, IS_STRING, buf, len, length, &retlen);
		if (!line) {
			if (buf.v) {
				efree(buf.v);
			}
			RETURN_FALSE;
		}

		if (allow) {
			convert_to_string_ex(allow);
			allowed = Z_STRVAL_PP(allow);
			allowed_len = Z_STRLEN_PP(allow);
		}
		retlen = php_strip_tags(line, retlen, &stream->fgetss_state, allowed, allowed_len);

		RETURN_STRINGL(line, retlen, 0);
	}
}
/* }}} */

/* {{{ proto mixed fscanf(resource stream, string format [, string ...]) U
   Implements a mostly ANSI compatible fscanf() */
PHP_FUNCTION(fscanf)
{
	int type, result, argc = 0;
	zval ***args = NULL;
	zval **format;
	zval *file_handle;	
	char *buf;
	UChar *u_buf;
	void *what;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ*", &file_handle, &format, &args, &argc) == FAILURE) {
		return;
	}

	what = zend_fetch_resource(&file_handle TSRMLS_CC, -1, "File-Handle", &type, 2, php_file_le_stream(), php_file_le_pstream());

	/*
	 * we can't do a ZEND_VERIFY_RESOURCE(what), otherwise we end up
	 * with a leak if we have an invalid filehandle. This needs changing
	 * if the code behind ZEND_VERIFY_RESOURCE changed. - cc
	 */
	if (!what) {
		if (args) {
			efree(args);
		}
		RETURN_FALSE;
	}

	if (((php_stream *)what)->readbuf_type == IS_UNICODE) {
		u_buf = php_stream_u_get_line((php_stream *) what, NULL_ZSTR, 0, 0, NULL);
		if (u_buf == NULL) {
			if (args) {
				efree(args);
			}
			RETURN_FALSE;
		}

		convert_to_unicode_ex(format);
		result = php_u_sscanf_internal(u_buf, Z_USTRVAL_PP(format), argc, args, 0, &return_value TSRMLS_CC);
		efree(u_buf);
	} else {
		buf = php_stream_get_line((php_stream *) what, NULL_ZSTR, 0, NULL);
		if (buf == NULL) {
			if (args) {
				efree(args);
			}
			RETURN_FALSE;
		}

		convert_to_string_ex(format);
		result = php_sscanf_internal(buf, Z_STRVAL_PP(format), argc, args, 0, &return_value TSRMLS_CC);
		efree(buf);
	}

	if (args) {
		efree(args);
	}

	if (SCAN_ERROR_WRONG_PARAM_COUNT == result) {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto int fwrite(resource fp, string str [, int length]) U
   Binary-safe file write */
PHPAPI PHP_FUNCTION(fwrite)
{
	int ret, argc = ZEND_NUM_ARGS();
	long write_len = -1;
	php_stream *stream;
	zval *zstream, *zstring;

	if (zend_parse_parameters(argc TSRMLS_CC, "rz/|l", &zstream, &zstring, &write_len) == FAILURE) {
		RETURN_NULL();
	}

	php_stream_from_zval(stream, &zstream);
	
	if (argc > 2 && write_len <= 0) {
		RETURN_LONG(0);
	}

	if (Z_TYPE_P(zstring) == IS_UNICODE) {
		int32_t write_uchars = 0;

		if (write_len < 0 || write_len > Z_USTRLEN_P(zstring)) {
			write_len = Z_USTRLEN_P(zstring);
		}
		/* Convert code units to data points */

		U16_FWD_N(Z_USTRVAL_P(zstring), write_uchars, Z_USTRLEN_P(zstring), write_len);
		write_len = write_uchars;

		ret = php_stream_write_unicode(stream, Z_USTRVAL_P(zstring), write_len);

		/* Convert data points back to code units */
		if (ret > 0) {
			ret = u_countChar32(Z_USTRVAL_P(zstring), ret);
		}
	} else {
		convert_to_string(zstring);
		if (write_len < 0 || write_len > Z_STRLEN_P(zstring)) {
			write_len = Z_STRLEN_P(zstring);
		}

		ret = php_stream_write(stream, Z_STRVAL_P(zstring), write_len);
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto bool fflush(resource fp) U
   Flushes output */
PHPAPI PHP_FUNCTION(fflush)
{
	zval *arg1;
	int ret;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	ret = php_stream_flush(stream);
	if (ret) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool rewind(resource fp) U
   Rewind the position of a file pointer */
PHPAPI PHP_FUNCTION(rewind)
{
	zval *arg1;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	if (-1 == php_stream_rewind(stream)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ftell(resource fp) U
   Get file pointer's read/write position */
PHPAPI PHP_FUNCTION(ftell)
{
	zval *arg1;
	long ret;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	ret = php_stream_tell(stream);
	if (ret == -1)	{
		RETURN_FALSE;
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int fseek(resource fp, int offset [, int whence]) U
   Seek on a file pointer */
PHPAPI PHP_FUNCTION(fseek)
{
	zval *arg1;
	long arg2, arg3;
	int whence = SEEK_SET, argcount = ZEND_NUM_ARGS();
	php_stream *stream;

	if (zend_parse_parameters(argcount TSRMLS_CC, "rl|l", &arg1, &arg2, &arg3) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	if (argcount > 2) {
		whence = arg3;
	}

	RETURN_LONG(php_stream_seek(stream, arg2, whence));
}
/* }}} */

/* {{{ php_mkdir
*/

/* DEPRECATED APIs: Use php_stream_mkdir() instead */
PHPAPI int php_mkdir_ex(char *dir, long mode, int options TSRMLS_DC)
{
	int ret;

	if (php_check_open_basedir(dir TSRMLS_CC)) {
		return -1;
	}

	if ((ret = VCWD_MKDIR(dir, (mode_t)mode)) < 0 && (options & REPORT_ERRORS)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
	}

	return ret;
}

PHPAPI int php_mkdir(char *dir, long mode TSRMLS_DC)
{
	return php_mkdir_ex(dir, mode, REPORT_ERRORS TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool mkdir(string pathname [, int mode [, bool recursive [, resource context]]]) U
   Create a directory */
PHP_FUNCTION(mkdir)
{
	zval **ppdir;
	char *dir;
	int dir_len;
	zval *zcontext = NULL;
	long mode = 0777;
	zend_bool recursive = 0;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|lbr", &ppdir, &mode, &recursive, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);
	if (php_stream_path_param_encode(ppdir, &dir, &dir_len, REPORT_ERRORS, context) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_BOOL(php_stream_mkdir(dir, mode, (recursive ? PHP_STREAM_MKDIR_RECURSIVE : 0) | REPORT_ERRORS, context));
}
/* }}} */

/* {{{ proto bool rmdir(string dirname[, resource context]) U
   Remove a directory */
PHP_FUNCTION(rmdir)
{
	zval **ppdir;
	char *dir;
	int dir_len;
	zval *zcontext = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|r", &ppdir, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);
	if (php_stream_path_param_encode(ppdir, &dir, &dir_len, REPORT_ERRORS, context) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_BOOL(php_stream_rmdir(dir, REPORT_ERRORS, context));
}
/* }}} */

/* {{{ proto int readfile(string filename [, int flags[, resource context]]) U
   Output a file or a URL */
PHP_FUNCTION(readfile)
{
	zval **ppfilename;
	char *filename;
	int filename_len;
	int size = 0;
	long flags = 0;
	zval *zcontext = NULL;
	php_stream *stream;
	php_stream_context *context = NULL;
	char *mode = "rb";

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|lr!", &ppfilename, &flags, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);
	if (php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, context) == FAILURE) {
		RETURN_FALSE;
	}

	if (flags & PHP_FILE_TEXT) {
		mode = "rt";
	}

	stream = php_stream_open_wrapper_ex(filename, mode, ((flags & PHP_FILE_USE_INCLUDE_PATH) ? USE_PATH : 0) | REPORT_ERRORS, NULL, context);
	if (stream) {
		size = php_stream_passthru(stream);
		php_stream_close(stream);
		RETURN_LONG(size);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int umask([int mask]) U
   Return or change the umask */
PHP_FUNCTION(umask)
{
	long arg1 = 0;
	int oldumask;
	int arg_count = ZEND_NUM_ARGS();

	oldumask = umask(077);

	if (BG(umask) == -1) {
		BG(umask) = oldumask;
	}
	
	if (zend_parse_parameters(arg_count TSRMLS_CC, "|l", &arg1) == FAILURE) {
		return;
	}

	if (arg_count == 0) {
		umask(oldumask);
	} else {
		umask(arg1);
	}

	RETURN_LONG(oldumask);
}
/* }}} */

/* {{{ proto int fpassthru(resource fp) U
   Output all remaining data from a file pointer */
PHPAPI PHP_FUNCTION(fpassthru)
{
	zval *arg1;
	int size;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	size = php_stream_passthru(stream);
	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto bool rename(string old_name, string new_name[, resource context]) U
   Rename a file */
PHP_FUNCTION(rename)
{
	zval **ppold, **ppnew;
	char *old_name, *new_name;
	int old_name_len, new_name_len;
	zval *zcontext = NULL;
	php_stream_wrapper *wrapper;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ|r", &ppold, &ppnew, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);
	if (php_stream_path_param_encode(ppold, &old_name, &old_name_len, REPORT_ERRORS, context) == FAILURE ||
		php_stream_path_param_encode(ppnew, &new_name, &new_name_len, REPORT_ERRORS, context) == FAILURE) {
		RETURN_FALSE;
	}

	wrapper = php_stream_locate_url_wrapper(old_name, NULL, 0 TSRMLS_CC);

	if (!wrapper || !wrapper->wops) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to locate stream wrapper");
		RETURN_FALSE;
	}

	if (!wrapper->wops->rename) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s wrapper does not support renaming", wrapper->wops->label ? wrapper->wops->label : "Source");
		RETURN_FALSE;
	}

	if (wrapper != php_stream_locate_url_wrapper(new_name, NULL, 0 TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot rename a file across wrapper types");
		RETURN_FALSE;
	}

	RETVAL_BOOL(wrapper->wops->rename(wrapper, old_name, new_name, 0, context TSRMLS_CC));
}
/* }}} */

/* {{{ proto bool unlink(string filename[, context context]) U
   Delete a file */
PHP_FUNCTION(unlink)
{
	zval **ppfilename;
	char *filename;
	int filename_len;
	php_stream_wrapper *wrapper;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|r", &ppfilename, &zcontext) == FAILURE) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);
	if (php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, context) == FAILURE) {
		RETURN_FALSE;
	}

	wrapper = php_stream_locate_url_wrapper(filename, NULL, 0 TSRMLS_CC);

	if (!wrapper || !wrapper->wops) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to locate stream wrapper");
		RETURN_FALSE;
	}

	if (!wrapper->wops->unlink) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s does not allow unlinking", wrapper->wops->label ? wrapper->wops->label : "Wrapper");
		RETURN_FALSE;
	}

	RETVAL_BOOL(wrapper->wops->unlink(wrapper, filename, REPORT_ERRORS, context TSRMLS_CC));
}
/* }}} */

/* {{{ proto bool ftruncate(resource fp, int size) U
   Truncate file to 'size' length */
PHP_NAMED_FUNCTION(php_if_ftruncate)
{
	zval *fp;
	long size;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &fp, &size) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &fp);

	if (!php_stream_truncate_supported(stream)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't truncate this stream!");
		RETURN_FALSE;
	}

	RETURN_BOOL(0 == php_stream_truncate_set_size(stream, size));
}
/* }}} */

/* {{{ proto array fstat(resource fp) U
   Stat() on a filehandle */
PHP_NAMED_FUNCTION(php_if_fstat)
{
	zval *fp;
	zval *stat_dev, *stat_ino, *stat_mode, *stat_nlink, *stat_uid, *stat_gid, *stat_rdev,
		 *stat_size, *stat_atime, *stat_mtime, *stat_ctime, *stat_blksize, *stat_blocks;
	php_stream *stream;
	php_stream_statbuf stat_ssb;
	char *stat_sb_names[13] = {
		"dev", "ino", "mode", "nlink", "uid", "gid", "rdev",
		"size", "atime", "mtime", "ctime", "blksize", "blocks"
	};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &fp) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &fp);

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
#ifdef NETWARE
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
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[0], strlen(stat_sb_names[0])+1, (void *)&stat_dev, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[1], strlen(stat_sb_names[1])+1, (void *)&stat_ino, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[2], strlen(stat_sb_names[2])+1, (void *)&stat_mode, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[3], strlen(stat_sb_names[3])+1, (void *)&stat_nlink, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[4], strlen(stat_sb_names[4])+1, (void *)&stat_uid, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[5], strlen(stat_sb_names[5])+1, (void *)&stat_gid, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[6], strlen(stat_sb_names[6])+1, (void *)&stat_rdev, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[7], strlen(stat_sb_names[7])+1, (void *)&stat_size, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[8], strlen(stat_sb_names[8])+1, (void *)&stat_atime, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[9], strlen(stat_sb_names[9])+1, (void *)&stat_mtime, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[10], strlen(stat_sb_names[10])+1, (void *)&stat_ctime, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[11], strlen(stat_sb_names[11])+1, (void *)&stat_blksize, sizeof(zval *), NULL);
	zend_ascii_hash_update(HASH_OF(return_value), stat_sb_names[12], strlen(stat_sb_names[12])+1, (void *)&stat_blocks, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ proto bool copy(string source_file, string destination_file[, resource context]) U
   Copy a file */
PHP_FUNCTION(copy)
{
	zval **source, **target, *zcontext = NULL;
	php_stream_context *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ|r", &source, &target, &zcontext) == FAILURE) {
		return;
	}

	context = php_stream_context_from_zval(zcontext, 0);
	if (FAILURE == php_stream_path_param_encode(source, NULL, NULL, REPORT_ERRORS, context) ||
		FAILURE == php_stream_path_param_encode(target, NULL, NULL, REPORT_ERRORS, context) ||
		0 != php_check_open_basedir(Z_STRVAL_PP(source) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (php_copy_file(Z_STRVAL_PP(source), Z_STRVAL_PP(target) TSRMLS_CC) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

PHPAPI int php_copy_file(char *src, char *dest TSRMLS_DC) /* {{{ */
{
	return php_copy_file_ex(src, dest, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ php_copy_file
 */
PHPAPI int php_copy_file_ex(char *src, char *dest, int src_chk TSRMLS_DC)
{
	php_stream *srcstream = NULL, *deststream = NULL;
	int ret = FAILURE;
	php_stream_statbuf src_s, dest_s;

	switch (php_stream_stat_path_ex(src, 0, &src_s, NULL)) {
		case -1:
			/* non-statable stream */
			goto safe_to_copy;
			break;
		case 0:
			break;
		default: /* failed to stat file, does not exist? */
			return ret;
	}
	if (S_ISDIR(src_s.sb.st_mode)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The first argument to copy() function cannot be a directory");
		return FAILURE;
	}
	
	switch (php_stream_stat_path_ex(dest, PHP_STREAM_URL_STAT_QUIET, &dest_s, NULL)) {
		case -1:
			/* non-statable stream */
			goto safe_to_copy;
			break;
		case 0:
			break;
		default: /* failed to stat file, does not exist? */
			return ret;
	}
	if (S_ISDIR(dest_s.sb.st_mode)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The second argument to copy() function cannot be a directory");
		return FAILURE;
	}
	if (!src_s.sb.st_ino || !dest_s.sb.st_ino) {
		goto no_stat;
	}
	if (src_s.sb.st_ino == dest_s.sb.st_ino && src_s.sb.st_dev == dest_s.sb.st_dev) {
		return ret;
	} else {
		goto safe_to_copy;
	}
no_stat:
	{
		char *sp, *dp;
		int res;

		if ((sp = expand_filepath(src, NULL TSRMLS_CC)) == NULL) {
			return ret;
		}
		if ((dp = expand_filepath(dest, NULL TSRMLS_CC)) == NULL) {
			efree(sp);
			goto safe_to_copy;
		}

		res =
#ifndef PHP_WIN32
			!strcmp(sp, dp);
#else
			!strcasecmp(sp, dp);
#endif

		efree(sp);
		efree(dp);
		if (res) {
			return ret;
		}
	}
safe_to_copy:

	srcstream = php_stream_open_wrapper(src, "rb", src_chk | REPORT_ERRORS, NULL);

	if (!srcstream) {
		return ret;
	}

	deststream = php_stream_open_wrapper(dest, "wb", REPORT_ERRORS, NULL);

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

/* {{{ proto string fread(resource fp, int length) U
   Binary-safe file read */
PHPAPI PHP_FUNCTION(fread)
{
	zval *zstream;
	long len;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zstream, &len) == FAILURE) {
		RETURN_NULL();
	}

	php_stream_from_zval(stream, &zstream);

	if (len <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter must be greater than 0");
		RETURN_FALSE;
	}

	if (stream->readbuf_type == IS_UNICODE) {
		int buflen = len;
		UChar *buf = php_stream_read_unicode_chars(stream, &buflen);

		RETURN_UNICODEL(buf, buflen, 0);
	} else { /* IS_STRING */
		char *buf = emalloc(len + 1);
		int buflen = php_stream_read(stream, buf, len);

		buf[buflen] = 0;
		RETURN_STRINGL(buf, buflen, 0);
	}
}
/* }}} */

#define FPUTCSV_FLD_CHK(c) memchr(Z_STRVAL(field), c, Z_STRLEN(field))

/* {{{ proto int fputcsv(resource fp, array fields [, string delimiter [, string enclosure]])
   Format line as CSV and write to file pointer */
/* UTODO: Output unicode contents */
PHP_FUNCTION(fputcsv)
{
	char delimiter = ',';	/* allow this to be set as parameter */
	char enclosure = '"';	/* allow this to be set as parameter */
	const char escape_char = '\\';
	php_stream *stream;
	int ret;
	zval *fp = NULL, *fields = NULL, **field_tmp = NULL, field;
	char *delimiter_str = NULL, *enclosure_str = NULL;
	int delimiter_str_len = 0, enclosure_str_len = 0;
	HashPosition pos;
	int count, i = 0;
	smart_str csvline = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra|ss",
			&fp, &fields, &delimiter_str, &delimiter_str_len,
			&enclosure_str, &enclosure_str_len) == FAILURE) {
		return;
	}

	if (delimiter_str != NULL) {
		/* Make sure that there is at least one character in string */
		if (delimiter_str_len < 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "delimiter must be a character");
			RETURN_FALSE;
		} else if (delimiter_str_len > 1) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "delimiter must be a single character");
		}

		/* use first character from string */
		delimiter = *delimiter_str;
	}

	if (enclosure_str != NULL) {
		if (enclosure_str_len < 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "enclosure must be a character");
			RETURN_FALSE;
		} else if (enclosure_str_len > 1) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "enclosure must be a single character");
		}
		/* use first character from string */
		enclosure = *enclosure_str;
	}

	PHP_STREAM_TO_ZVAL(stream, &fp);

	count = zend_hash_num_elements(Z_ARRVAL_P(fields));
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(fields), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(fields), (void **) &field_tmp, &pos) == SUCCESS) {
		field = **field_tmp;

		if (Z_TYPE_PP(field_tmp) != IS_STRING) {
			zval_copy_ctor(&field);
			convert_to_string(&field);
		}

		/* enclose a field that contains a delimiter, an enclosure character, or a newline */
		if (FPUTCSV_FLD_CHK(delimiter) ||
			FPUTCSV_FLD_CHK(enclosure) ||
			FPUTCSV_FLD_CHK(escape_char) ||
			FPUTCSV_FLD_CHK('\n') ||
			FPUTCSV_FLD_CHK('\r') ||
			FPUTCSV_FLD_CHK('\t') ||
			FPUTCSV_FLD_CHK(' ')
		) {
			char *ch = Z_STRVAL(field);
			char *end = ch + Z_STRLEN(field);
			int escaped = 0;

			smart_str_appendc(&csvline, enclosure);
			while (ch < end) {
				if (*ch == escape_char) {
					escaped = 1;
				} else if (!escaped && *ch == enclosure) {
					smart_str_appendc(&csvline, enclosure);
				} else {
					escaped = 0;
				}
				smart_str_appendc(&csvline, *ch);
				ch++;
			}
			smart_str_appendc(&csvline, enclosure);
		} else {
			smart_str_appendl(&csvline, Z_STRVAL(field), Z_STRLEN(field));
		}

		if (++i != count) {
			smart_str_appendl(&csvline, &delimiter, 1);
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(fields), &pos);
		
		if (Z_TYPE_PP(field_tmp) != IS_STRING) {
			zval_dtor(&field);
		}
	}

	smart_str_appendc(&csvline, '\n');
	smart_str_0(&csvline);

	ret = php_stream_write(stream, csvline.c, csvline.len);
	smart_str_free(&csvline);

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto array fgetcsv(resource fp [,int length [, string delimiter [, string enclosure[, string escape]]]]) U
   Get line from file pointer and parse for CSV fields */
#define PHP_FGETCSV_TRUNCATE(field) \
if (argc > 4) { \
	/* Caller knows about new semantics since they're using new param, allow multichar */ \
} else if (field##_type == IS_STRING && field##_len > 1) { \
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, #field " must be a single character"); \
	delimiter_len = 1; \
} else if (field##_type == IS_UNICODE && u_countChar32((UChar*)field, field##_len) > 1) { \
	int __tmp = 0; \
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, #field " must be a single character"); \
	U16_FWD_1(((UChar*)field), __tmp, field##_len); \
	field##_len = __tmp; \
}

PHP_FUNCTION(fgetcsv)
{
	zend_uchar delimiter_type = IS_STRING, enclosure_type = IS_STRING, escape_type = IS_STRING;
	char *delimiter = ",", *enclosure = "\"", *escape = "\\";
	int delimiter_len = 1, enclosure_len = 1, escape_len = 1;
	long len = -1;
	zstr buf;
	size_t buf_len;
	int argc = ZEND_NUM_ARGS();
	php_stream *stream;
	zval *zstream;
	zend_uchar delimiter_free = 0, enclosure_free = 0, escape_free = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "r|lttt", &zstream, &len,
						&delimiter, &delimiter_len, &delimiter_type,
						&enclosure, &enclosure_len, &enclosure_type,
						&escape,    &escape_len,    &escape_type) == FAILURE) {
		return;
	}

	PHP_STREAM_TO_ZVAL(stream, &zstream);

	/* Make sure that there is at least one character in string,
	 * For userspace BC purposes we generally limit delimiters and enclosures to 1 character,
	 * though the code now supports multiple characters
	 *
	 * If this function is called with all five parameters however,
	 * then multiple characters are allowed for all subarguments */
	if (delimiter_len < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "delimiter must be a character");
		RETURN_FALSE;
	} else PHP_FGETCSV_TRUNCATE(delimiter);

	if (enclosure_len < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "enclosure must be a character");
		RETURN_FALSE;
	} else PHP_FGETCSV_TRUNCATE(enclosure);

	if (escape_len < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "escape must be a character");
		RETURN_FALSE;
	}

	if (len < -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length parameter may not be negative");
		RETURN_FALSE;
	} else if (len == 0) {
		len = -1;
	}

	if (stream->readbuf_type == IS_STRING) {
		/* Binary mode stream needs binary delimiter/enclosure */
		if (delimiter_type == IS_UNICODE) {
			if (FAILURE == zend_unicode_to_string(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &delimiter, &delimiter_len, (UChar*)delimiter, delimiter_len TSRMLS_CC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed converting delimiter from unicode");
				RETVAL_FALSE;
				goto cleanup;
			}
			delimiter_free = 1;
		}
		if (enclosure_type == IS_UNICODE) {
			if (FAILURE == zend_unicode_to_string(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &enclosure, &enclosure_len, (UChar*)enclosure, enclosure_len TSRMLS_CC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed converting enclosure from unicode");
				RETVAL_FALSE;
				goto cleanup;
			}
			enclosure_free = 1;
		}
		if (escape_type == IS_UNICODE) {
			if (FAILURE == zend_unicode_to_string(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &escape, &escape_len, (UChar*)escape, escape_len TSRMLS_CC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed converting escape from unicode");
				RETVAL_FALSE;
				goto cleanup;
			}
			escape_free = 1;
		}
	} else {
		/* Unicode mode stream needs unicode delimiter/enclosure */
		if (delimiter_type == IS_STRING) {
			if (FAILURE == zend_string_to_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), (UChar**)&delimiter, &delimiter_len, delimiter, delimiter_len TSRMLS_CC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed converting delimiter to unicode");
				RETVAL_FALSE;
				goto cleanup;
			}
			delimiter_free = 1;
		}
		if (enclosure_type == IS_STRING) {
			if (FAILURE == zend_string_to_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), (UChar**)&enclosure, &enclosure_len, enclosure, enclosure_len TSRMLS_CC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed converting enclosure to unicode");
				RETVAL_FALSE;
				goto cleanup;
			}
			enclosure_free = 1;
		}
		if (escape_type == IS_STRING) {
			if (FAILURE == zend_string_to_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), (UChar**)&escape, &escape_len, escape, escape_len TSRMLS_CC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed converting escape to unicode");
				RETVAL_FALSE;
				goto cleanup;
			}
			escape_free = 1;
		}
	}

	if (len < 0) {
		buf.v = php_stream_get_line_ex(stream, stream->readbuf_type, NULL_ZSTR, 0, 0, &buf_len);
	} else {
		buf.v = stream->readbuf_type == IS_UNICODE ? eumalloc(len + 1) : emalloc(len + 1);
		if (php_stream_get_line_ex(stream, stream->readbuf_type, buf, len + 1, len + 1, &buf_len) == NULL) {
			efree(buf.v);
			RETVAL_FALSE;
			goto cleanup;	
		}
	}

	if (!buf.v) {
		/* No data */
		RETVAL_FALSE;
		goto cleanup;
	}

	if (stream->readbuf_type == IS_UNICODE) {
		/* Unicode mode */
		php_u_fgetcsv(stream, (UChar*)delimiter, delimiter_len, (UChar*)enclosure, enclosure_len, (UChar*)escape, escape_len, buf.u, buf_len, return_value TSRMLS_CC);
	} else {
		/* Binary mode */
		php_fgetcsv_ex(stream, delimiter, delimiter_len, enclosure, enclosure_len, escape, escape_len, buf.s, buf_len, return_value TSRMLS_CC);
	}

cleanup:
	if (delimiter_free) {
		efree(delimiter);
	}
	if (enclosure_free) {
		efree(enclosure);
	}
	if (escape_free) {
		efree(escape);
	}
}
/* }}} */

PHPAPI void php_fgetcsv(php_stream *stream, char delimiter, char enclosure, char escape, size_t buf_len, char *buf, zval *return_value TSRMLS_DC) /* {{{ */
{
	char *delim = &delimiter, *enc = &enclosure, *buffer = buf, *esc = &escape;
	int delim_len = 1, enc_len = 1, esc_len = 1, buffer_len = buf_len;
	zend_uchar type = IS_STRING;

	if (stream) {
		type = stream->readbuf_type;
	}

	if (type == IS_UNICODE) {

		/* Unicode stream, but binary delimiter/enclosures/prefetch, promote to unicode */
		if (FAILURE == zend_string_to_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), (UChar**)&delim, &delim_len, &delimiter, 1 TSRMLS_CC)) {
			INIT_PZVAL(return_value);
			return;
		}
		if (FAILURE == zend_string_to_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), (UChar**)&enc, &enc_len, &enclosure, 1 TSRMLS_CC)) {
			efree(delim);
			INIT_PZVAL(return_value);
			return;
		}
		if (FAILURE == zend_string_to_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), (UChar**)&esc, &esc_len, &escape, 1 TSRMLS_CC)) {
			efree(delim);
			efree(enc);
			INIT_PZVAL(return_value);
			return;
		}
		if (FAILURE == zend_string_to_unicode(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), (UChar**)&buffer, &buffer_len, buf, buf_len TSRMLS_CC)) {
			efree(delim);
			efree(enc);
			efree(esc);
			INIT_PZVAL(return_value);
			return;
		}

		php_u_fgetcsv(stream, (UChar*)delim, delim_len, (UChar*)enc, enc_len, (UChar*)esc, esc_len,
				(UChar*)buffer, buffer_len, return_value TSRMLS_CC);

		/* Types converted, free storage */
		efree(delim);
		efree(enc);
		efree(esc);
	} else {
		/* Binary stream with binary delimiter/enclosures/prefetch */
		php_fgetcsv_ex(stream, delim, delim_len, enc, enc_len, esc, esc_len, buffer, buffer_len, return_value TSRMLS_CC);
	}
}

typedef enum _php_fgetcsv_state {
	PHP_FGETCSV_READY,
	PHP_FGETCSV_FIELD_NO_ENC,
	PHP_FGETCSV_FIELD_WITH_ENC,
	PHP_FGETCSV_POST_ENC,
} php_fgetcsv_state;

#define PHP_FGETCSV_BIN_CHECK(p, e, m, mlen) ((p) < (e) && (((mlen) == 1 && *(p) == *(m)) || ((mlen) > 1 && (((e) - (p)) >= (mlen)) && memcmp((p), (m), (mlen)) == 0)))

/* Binary mode fgetcsv */
PHPAPI void php_fgetcsv_ex(php_stream *stream,
		char *delimiter, int delimiter_len,
		char *enclosure, int enclosure_len,
		char *escape, int escape_len,
		char *buffer, int buffer_len,
		zval *return_value TSRMLS_DC)
{
	php_fgetcsv_state state = PHP_FGETCSV_READY;
	char *p = buffer, *e = buffer + buffer_len, *field_start = NULL, *field_end = NULL;

	array_init(return_value);

	while(p < e) {
		switch (state) {
			case PHP_FGETCSV_READY:
ready_state:
				/* Ready to start a new field */

				/* Is there nothing left to scan? */
				if (*p == '\r' || *p == '\n') {
					/* Terminal delimiter, treat as empty field */
					p++;
					add_next_index_stringl(return_value, "", 0, 1);
					break;
				}

				/* Is it enclosed? */
				if (PHP_FGETCSV_BIN_CHECK(p, e, enclosure, enclosure_len)) {
					/* Enclosure encountered, switch state */
					state = PHP_FGETCSV_FIELD_WITH_ENC;
					p += enclosure_len;
					field_start = p;
					break;
				}

				/* Is it an immediate delimiter? */
				if (PHP_FGETCSV_BIN_CHECK(p, e, delimiter, delimiter_len)) {
					/* Immediate delimiter, treate as empty field */
					p += delimiter_len;
					add_next_index_stringl(return_value, "", 0, 1);
					break;
				}

				/* Whitespace? */
				if (*p == ' ' || *p == '\t') {
					p++;
					if (p >= e) break;
					goto ready_state;
				}
				
				/* Otherwise, starting a new field without enclosures */
				state = PHP_FGETCSV_FIELD_NO_ENC;
				field_start = p++;
				field_end = NULL;

				/* Is it an escape character? */
				if ((PHP_FGETCSV_BIN_CHECK(p, e, escape, escape_len) && escape != enclosure) ||
					(PHP_FGETCSV_BIN_CHECK(p, e, escape, escape_len) &&
					PHP_FGETCSV_BIN_CHECK(p+1, e, escape, escape_len) && escape == enclosure)
				) {
					/* Skip escape sequence and let next char be treated as literal
					 * If enclosure is the same character as esacpe, it is considered as esacped
					 * if it appears twice */
					p += escape_len - 1;
					/* FALL THROUGH */
				}
				break;

			case PHP_FGETCSV_FIELD_WITH_ENC:
with_enc:
				/* Check for ending enclosure */
				if (PHP_FGETCSV_BIN_CHECK(p, e, enclosure, enclosure_len)) {
					/* Enclosure encountered, is it paired? */
					if (PHP_FGETCSV_BIN_CHECK(p + enclosure_len, e, enclosure, enclosure_len)) {
						/* Double enclosure gets translated to single enclosure */
						memmove(p, p + enclosure_len, (e - p) - enclosure_len);
						e -= enclosure_len;
						p += enclosure_len;
						goto with_enc;
					} else {
						/* Genuine end enclosure, switch state */
						field_end = p;
						p += enclosure_len;
						state = PHP_FGETCSV_POST_ENC;
						goto post_enc;
					}
				}

				/* Check for field escapes */
				if (PHP_FGETCSV_BIN_CHECK(p, e, escape, escape_len)) {
					p += escape_len + 1;

					/* Reprocess for ending enclosures */
					goto with_enc;
				}

				/* Simple character */
				if (e - p) {
					p++;
				}

				/* Hungry? */
				if (((e - p) < enclosure_len) && stream) {
					/* Feed me! */
					size_t new_len;
					char *new_buf = php_stream_get_line(stream, NULL_ZSTR, 0, &new_len);

					if (new_buf) {
						int tmp_len = new_len + e - field_start;
						char *tmp = emalloc(tmp_len);

						/* Realign scan buffer */
						memcpy(tmp, field_start, e - field_start);
						memcpy(tmp + (e - field_start), new_buf, new_len);
						field_start = tmp;
						if (field_end) {
							field_end = tmp + (field_end - field_start);
						}
						efree(buffer);
						efree(new_buf);
						buffer = tmp;
						buffer_len = tmp_len;
						p = buffer;
						e = buffer + buffer_len;
					}
				}

				if ((e - p) == 0) {
					/* Nothing left to consume the buffer, use it */
					add_next_index_stringl(return_value, field_start, p - field_start, 1);

					/* Loop is dying anyway, but be pedantic */
					state = PHP_FGETCSV_READY;
					field_start = field_end = NULL;
					break;
				}
				break;

			case PHP_FGETCSV_POST_ENC:
post_enc:
				/* Check for delimiters or EOL */
				if (p >= e || *p == '\r' || *p == '\n' || PHP_FGETCSV_BIN_CHECK(p, e, delimiter, delimiter_len)) {
					int field_len = field_end - field_start;
					char *field;

					if ((p - enclosure_len) > field_end) {
						/* There's cruft, append it to the proper field */
						int cruft_len = p - (field_end + enclosure_len);

						field = emalloc(field_len + cruft_len + 1);
						memcpy(field, field_start, field_len);
						memcpy(field + field_len, field_end + enclosure_len, cruft_len);

						field_len += cruft_len;
						field[field_len] = 0;
					} else {
						field = estrndup(field_start, field_end - field_start);
					}
					add_next_index_stringl(return_value, field, field_len, 0);

					/* Reset scanner */
					state = PHP_FGETCSV_READY;
					field_start = field_end = NULL;
					p += delimiter_len;
					if (p >= e) break;
					goto ready_state;
				}

				/* Queue anything else as cruft */
				p++;
				break;

			case PHP_FGETCSV_FIELD_NO_ENC:
				/* Check for escapes */
				if (!PHP_FGETCSV_BIN_CHECK(p, e, delimiter, delimiter_len) && PHP_FGETCSV_BIN_CHECK(p, e, escape, escape_len)) {
					p += escape_len + 1;
				}

				/* Check for delimiter */
				if (p >= e || *p == '\r' || *p == '\n' || PHP_FGETCSV_BIN_CHECK(p, e, delimiter, delimiter_len)) {
					add_next_index_stringl(return_value, field_start, p - field_start, 1);

					/* Reset scanner */
					state = PHP_FGETCSV_READY;
					field_start = field_end = NULL;
					p += delimiter_len;
					if (p >= e) break;
					goto ready_state;
				}

				/* Simple character */
				p++;

				if (p == e) {
					add_next_index_stringl(return_value, field_start, p - field_start, 1);
					/* Reset scanner even though we're dying */
					state = PHP_FGETCSV_READY;
					field_start = field_end = NULL;
					p += delimiter_len;
				}
				break;
		}
	}

	if (stream) {
		efree(buffer);
	}
}
/* }}} */

#define PHP_FGETCSV_UNI_CHECK(p, e, m, mlen) ((p) < (e) && (((mlen) == 1 && *(p) == *(m)) || ((mlen) > 1 && (((e) - (p)) >= (mlen)) && memcmp((p), (m), UBYTES(mlen)) == 0)))

/* Unicode mode fgetcsv */
PHPAPI void php_u_fgetcsv(php_stream *stream, UChar *delimiter, int delimiter_len, UChar *enclosure, int enclosure_len, UChar *escape, int escape_len, UChar *buffer, int buffer_len, zval *return_value TSRMLS_DC) /* {{{ */
{
	php_fgetcsv_state state = PHP_FGETCSV_READY;
	UChar *p = buffer, *e = buffer + buffer_len, *field_start = NULL, *field_end = NULL;

	array_init(return_value);

	while(p < e) {
		switch (state) {
			case PHP_FGETCSV_READY:
ready_state:
				/* Ready to start a new field */

				/* Is there nothing left to scan? */
				if (*p == '\r' || *p == '\n') {
					/* Terminal delimiter, treat as empty field */
					p++;
					add_next_index_unicodel(return_value, (UChar*)"", 0, 1);
					break;
				}

				/* Is it enclosed? */
				if (PHP_FGETCSV_UNI_CHECK(p, e, enclosure, enclosure_len)) {
					/* Enclosure encountered, switch state */
					state = PHP_FGETCSV_FIELD_WITH_ENC;
					p += enclosure_len;
					field_start = p;
					break;
				}

				/* Is it an immediate delimiter? */
				if (PHP_FGETCSV_UNI_CHECK(p, e, delimiter, delimiter_len)) {
					/* Immediate delimiter, treate as empty field */
					p += delimiter_len;
					add_next_index_unicodel(return_value, (UChar*)"", 0, 1);
					break;
				}

				/* Whitespace? */
				if (*p == ' ' || *p == '\t') {
					p++;
					if (p >= e) break;
					goto ready_state;
				}
				
				/* Otherwise, starting a new field without enclosures */
				state = PHP_FGETCSV_FIELD_NO_ENC;
				field_start = p++;
				field_end = NULL;

				/* Is it an escape character? */
				if ((PHP_FGETCSV_UNI_CHECK(p, e, escape, escape_len) && escape != enclosure) ||
					(PHP_FGETCSV_UNI_CHECK(p, e, escape, escape_len) &&
					PHP_FGETCSV_UNI_CHECK(p+1, e, escape, escape_len) && escape == enclosure)
				) {
					/* Skip escape sequence and let next char be treated as literal
					 * If enclosure is the same character as escape, it is considered as escaped
					 * if it appears twice */
					p += escape_len - 1;
					/* FALL THROUGH */
				}
				break;

			case PHP_FGETCSV_FIELD_WITH_ENC:
with_enc:
				/* Check for ending enclosure */
				if (PHP_FGETCSV_UNI_CHECK(p, e, enclosure, enclosure_len)) {
					/* Enclosure encountered, is it paired? */
					if (PHP_FGETCSV_UNI_CHECK(p + enclosure_len, e, enclosure, enclosure_len)) {
						/* Double enclosure gets translated to single enclosure */
						memmove(p, p + enclosure_len, UBYTES((e - p) - enclosure_len));
						e -= enclosure_len;
						p += enclosure_len;
						if (p >= e) break;
						goto with_enc;
					} else {
						/* Genuine end enclosure, switch state */
						field_end = p;
						p += enclosure_len;
						state = PHP_FGETCSV_POST_ENC;
						goto post_enc;
					}
				}

				/* Check for field escapes */
				if (PHP_FGETCSV_UNI_CHECK(p, e, escape, escape_len)) {
					p += escape_len + 1;

					/* Reprocess for ending enclosures */
					if (p >= e) break;
					goto with_enc;
				}

				/* Simple character */
				if (e - p) {
					p++;
				}

				/* Hungry? */
				if (((e - p) < enclosure_len) && stream) {
					/* Feed me! */
					size_t new_len;
					UChar *new_buf = (UChar*)php_stream_get_line_ex(stream, IS_UNICODE, NULL_ZSTR, 0, 0, &new_len);

					if (new_buf) {
						int tmp_len = new_len + e - field_start;
						UChar *tmp = eumalloc(tmp_len);

						/* Realign scan buffer, ick -- expensive */
						memcpy(tmp, field_start, UBYTES(e - field_start));
						memcpy(tmp + (e - field_start), new_buf, UBYTES(new_len));
						field_start = tmp;
						if (field_end) {
							field_end = tmp + (field_end - field_start);
						}
						efree(buffer);
						efree(new_buf);
						buffer = tmp;
						buffer_len = tmp_len;
						p = buffer;
						e = buffer + buffer_len;
					}
				}

				if ((e - p) == 0) {
					/* Nothing left to consume the buffer */
					add_next_index_unicodel(return_value, field_start, p - field_start, 1);

					/* Loop is dying, but cleanup anyway */
					state = PHP_FGETCSV_READY;
					field_start = field_end = NULL;
					break;
				}
				break;

			case PHP_FGETCSV_POST_ENC:
post_enc:
				/* Check for delimiters or EOL */
				if (p >= e || *p == '\r' || *p == '\n' || PHP_FGETCSV_UNI_CHECK(p, e, delimiter, delimiter_len)) {
					int field_len = field_end - field_start;
					UChar *field;

					if ((p - enclosure_len) > field_end) {
						/* There's cruft, append it to the regular field */
						int cruft_len = p - (field_end + enclosure_len);

						field = eumalloc(field_len + cruft_len + 1);
						memcpy(field, field_start, UBYTES(field_len));
						memcpy(field + field_len, field_end + enclosure_len, UBYTES(cruft_len));
						field_len += cruft_len;
						field[field_len] = 0;
					} else {
						field = eustrndup(field_start, field_len);
					}
					add_next_index_unicodel(return_value, field, field_len, 0);

					/* Reset scanner state */
					state = PHP_FGETCSV_READY;
					field_start = field_end = NULL;
					p += delimiter_len;
					goto ready_state;
				}

				/* Queue anything else as cruft */
				p++;
				break;

			case PHP_FGETCSV_FIELD_NO_ENC:
				/* Check for escapes */
				if (!PHP_FGETCSV_UNI_CHECK(p, e, delimiter, delimiter_len) && PHP_FGETCSV_UNI_CHECK(p, e, escape, escape_len)) {
					p += escape_len + 1;
				}

				/* Check for delimiter */
				if (p >= e || *p == '\r' || *p == '\n' || PHP_FGETCSV_UNI_CHECK(p, e, delimiter, delimiter_len)) {
					add_next_index_unicodel(return_value, field_start, p - field_start, 1);
					state = PHP_FGETCSV_READY;
					field_start = field_end = NULL;
					p += delimiter_len;
					goto ready_state;
				}

				/* Simple character */
				p++;

				if (p == e) {
					add_next_index_unicodel(return_value, field_start, p - field_start, 1);
					/* Reset scanner even though we're dying */
					state = PHP_FGETCSV_READY;
					field_start = field_end = NULL;
					p += delimiter_len;
				}
				break;
		}
	}

	if (stream) {
		efree(buffer);
	}
}
/* }}} */

#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
/* {{{ proto string realpath(string path) U
   Return the resolved path */
PHP_FUNCTION(realpath)
{
	zval **ppfilename;
	char *filename;
	int filename_len;
	char resolved_path_buff[MAXPATHLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &ppfilename) == FAILURE ||
		php_stream_path_param_encode(ppfilename, &filename, &filename_len, REPORT_ERRORS, FG(default_context)) == FAILURE) {
		return;
	}

	if (VCWD_REALPATH(filename, resolved_path_buff)) {
#ifdef ZTS
		if (VCWD_ACCESS(resolved_path_buff, F_OK)) {
			RETVAL_FALSE;
		} else
#endif

		if (UG(unicode)) {
			UChar *path;
			int path_len;

			if (php_stream_path_decode(&php_plain_files_wrapper, &path, &path_len, resolved_path_buff, strlen(resolved_path_buff), REPORT_ERRORS, FG(default_context)) == SUCCESS) {
				RETVAL_UNICODEL(path, path_len, 0);
			} else {
				/* Fallback */
				RETVAL_STRING(resolved_path_buff, 1);
			}
		} else {
			RETVAL_STRING(resolved_path_buff, 1);
		}
	} else {
		RETVAL_FALSE;
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
		if (php_stream_eof(md->stream)) {
			break;
		}

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
				while (!php_stream_eof(md->stream) && (ch = php_stream_getc(md->stream)) && ch != compliment && ch != '<' && ch != '>') {
					buff[(md->token_len)++] = ch;

					if (md->token_len == META_DEF_BUFSIZE) {
						break;
					}
				}

				if (ch == '<' || ch == '>') {
					/* Was just an apostrohpe */
					md->ulc = 1;
					md->lc = ch;
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
					while (!php_stream_eof(md->stream) && (ch = php_stream_getc(md->stream)) && (isalnum(ch) || strchr(PHP_META_HTML401_CHARS, ch))) {
						buff[(md->token_len)++] = ch;

						if (md->token_len == META_DEF_BUFSIZE) {
							break;
						}
					}

					/* This is ugly, but we have to replace ungetc */
					if (!isalpha(ch) && ch != '-') {
						md->ulc = 1;
						md->lc = ch;
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
/* {{{ proto bool fnmatch(string pattern, string filename [, int flags]) U
   Match filename against pattern */
PHP_FUNCTION(fnmatch)
{
	zstr pattern, filename;
	int pattern_len, filename_len;
	char *pattern_utf8, *filename_utf8;
	int pattern_utf8_len, filename_utf8_len;
	zend_uchar type;
	long flags = 0;
	UErrorCode status = U_ZERO_ERROR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TT|l", &pattern, &pattern_len, &type, &filename, &filename_len, &type, &flags) == FAILURE) {
		return;
	}

	if (type == IS_UNICODE) {
		zend_unicode_to_string_ex(UG(utf8_conv), &pattern_utf8, &pattern_utf8_len, pattern.u, pattern_len, &status);
		zend_unicode_to_string_ex(UG(utf8_conv), &filename_utf8, &filename_utf8_len, filename.u, filename_len, &status);
		pattern.s = pattern_utf8;
		filename.s = filename_utf8;
		filename_len = filename_utf8_len;
	}
	if (filename_len >= MAXPATHLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filename exceeds the maximum allowed length of %d characters", MAXPATHLEN);
		RETURN_FALSE;
	}

	RETVAL_BOOL( ! fnmatch( pattern.s, filename.s, flags ));

	if (type == IS_UNICODE) {
		efree(pattern_utf8);
		efree(filename_utf8);
	}
}
/* }}} */
#endif

/* {{{ proto string sys_get_temp_dir() U
   Returns directory path used for temporary files */
PHP_FUNCTION(sys_get_temp_dir)
{
	UChar *utemp_dir;
	char *temp_dir = (char *)php_get_temporary_directory();
	int temp_dir_len = strlen(temp_dir), utemp_dir_len;

	if (!UG(unicode)) {
		RETURN_STRINGL(temp_dir, temp_dir_len, 1);
	}

	/* else UG(unicode) */
	if (FAILURE == php_stream_path_decode(NULL, &utemp_dir, &utemp_dir_len, temp_dir, temp_dir_len, REPORT_ERRORS, FG(default_context))) {
		RETURN_FALSE;
	}

	RETURN_UNICODEL(utemp_dir, utemp_dir_len, 0);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
