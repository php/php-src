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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Stig Bakken <ssb@php.net>                                   |
   |          Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   | PHP 4.0 patches by Thies C. Arntzen (thies@thieso.net)               |
   | PHP streams by Wez Furlong (wez@thebrainroom.com)                    |
   +----------------------------------------------------------------------+
*/

/* {{{ includes */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/flock_compat.h"
#include "ext/standard/exec.h"
#include "ext/standard/php_filestat.h"
#include "php_open_temporary_file.h"
#include "ext/standard/basic_functions.h"
#include "php_ini.h"
#include "zend_smart_str.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <wchar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef PHP_WIN32
# include <io.h>
# define O_RDONLY _O_RDONLY
# include "win32/param.h"
# include "win32/winutil.h"
# include "win32/fnmatch.h"
# include "win32/ioutil.h"
#else
# if HAVE_SYS_PARAM_H
#  include <sys/param.h>
# endif
# if HAVE_SYS_SELECT_H
#  include <sys/select.h>
# endif
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# if HAVE_ARPA_INET_H
#  include <arpa/inet.h>
# endif
#endif

#include "ext/standard/head.h"
#include "php_string.h"
#include "file.h"

#ifdef HAVE_PWD_H
# ifdef PHP_WIN32
#  include "win32/pwd.h"
# else
#  include <pwd.h>
# endif
#endif

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#include "fsock.h"
#include "fopen_wrappers.h"
#include "streamsfuncs.h"
#include "php_globals.h"

#ifdef HAVE_SYS_FILE_H
# include <sys/file.h>
#endif

#if MISSING_FCLOSE_DECL
extern int fclose(FILE *);
#endif

#ifdef HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif

#include "scanf.h"
#include "zend_API.h"

#ifdef ZTS
int file_globals_id;
#else
php_file_globals file_globals;
#endif

#if defined(HAVE_FNMATCH) && !defined(PHP_WIN32)
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include <fnmatch.h>
#endif

#include "file_arginfo.h"

/* }}} */

#define PHP_STREAM_FROM_ZVAL(stream, arg) \
	ZEND_ASSERT(Z_TYPE_P(arg) == IS_RESOURCE); \
	php_stream_from_res(stream, Z_RES_P(arg));

/* {{{ ZTS-stuff / Globals / Prototypes */

/* sharing globals is *evil* */
static int le_stream_context = FAILURE;

PHPAPI int php_le_stream_context(void)
{
	return le_stream_context;
}
/* }}} */

/* {{{ Module-Stuff */
static ZEND_RSRC_DTOR_FUNC(file_context_dtor)
{
	php_stream_context *context = (php_stream_context*)res->ptr;
	if (Z_TYPE(context->options) != IS_UNDEF) {
		zval_ptr_dtor(&context->options);
		ZVAL_UNDEF(&context->options);
	}
	php_stream_context_free(context);
}

static void file_globals_ctor(php_file_globals *file_globals_p)
{
	memset(file_globals_p, 0, sizeof(php_file_globals));
	file_globals_p->def_chunk_size = PHP_SOCK_CHUNK_SIZE;
}

static void file_globals_dtor(php_file_globals *file_globals_p)
{
#if defined(HAVE_GETHOSTBYNAME_R)
	if (file_globals_p->tmp_host_buf) {
		free(file_globals_p->tmp_host_buf);
	}
#endif
}

static PHP_INI_MH(OnUpdateAutoDetectLineEndings)
{
	if (zend_ini_parse_bool(new_value)) {
		zend_error(E_DEPRECATED, "auto_detect_line_endings is deprecated");
	}
	return OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("user_agent", NULL, PHP_INI_ALL, OnUpdateString, user_agent, php_file_globals, file_globals)
	STD_PHP_INI_ENTRY("from", NULL, PHP_INI_ALL, OnUpdateString, from_address, php_file_globals, file_globals)
	STD_PHP_INI_ENTRY("default_socket_timeout", "60", PHP_INI_ALL, OnUpdateLong, default_socket_timeout, php_file_globals, file_globals)
	STD_PHP_INI_BOOLEAN("auto_detect_line_endings", "0", PHP_INI_ALL, OnUpdateAutoDetectLineEndings, auto_detect_line_endings, php_file_globals, file_globals)
PHP_INI_END()

PHP_MINIT_FUNCTION(file)
{
	le_stream_context = zend_register_list_destructors_ex(file_context_dtor, NULL, "stream-context", module_number);

#ifdef ZTS
	ts_allocate_id(&file_globals_id, sizeof(php_file_globals), (ts_allocate_ctor) file_globals_ctor, (ts_allocate_dtor) file_globals_dtor);
#else
	file_globals_ctor(&file_globals);
#endif

	REGISTER_INI_ENTRIES();

	register_file_symbols(module_number);

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(file) /* {{{ */
{
#ifndef ZTS
	file_globals_dtor(&file_globals);
#endif
	return SUCCESS;
}
/* }}} */

PHPAPI void php_flock_common(php_stream *stream, zend_long operation,
	uint32_t operation_arg_num, zval *wouldblock, zval *return_value)
{
	int flock_values[] = { LOCK_SH, LOCK_EX, LOCK_UN };
	int act;

	act = operation & PHP_LOCK_UN;
	if (act < 1 || act > 3) {
		zend_argument_value_error(operation_arg_num, "must be one of LOCK_SH, LOCK_EX, or LOCK_UN");
		RETURN_THROWS();
	}

	if (wouldblock) {
		ZEND_TRY_ASSIGN_REF_LONG(wouldblock, 0);
	}

	/* flock_values contains all possible actions if (operation & PHP_LOCK_NB) we won't block on the lock */
	act = flock_values[act - 1] | (operation & PHP_LOCK_NB ? LOCK_NB : 0);
	if (php_stream_lock(stream, act)) {
		if (operation && errno == EWOULDBLOCK && wouldblock) {
			ZEND_TRY_ASSIGN_REF_LONG(wouldblock, 1);
		}
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* {{{ Portable file locking */
PHP_FUNCTION(flock)
{
	zval *res, *wouldblock = NULL;
	php_stream *stream;
	zend_long operation = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_RESOURCE(res)
		Z_PARAM_LONG(operation)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(wouldblock)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	php_flock_common(stream, operation, 2, wouldblock, return_value);
}
/* }}} */

#define PHP_META_UNSAFE ".\\+*?[^]$() "

/* {{{ Extracts all meta tag content attributes from a file and returns an array */
PHP_FUNCTION(get_meta_tags)
{
	char *filename;
	size_t filename_len;
	bool use_include_path = 0;
	int in_tag = 0, done = 0;
	int looking_for_val = 0, have_name = 0, have_content = 0;
	int saw_name = 0, saw_content = 0;
	char *name = NULL, *value = NULL, *temp = NULL;
	php_meta_tags_token tok, tok_last;
	php_meta_tags_data md;

	/* Initialize our structure */
	memset(&md, 0, sizeof(md));

	/* Parse arguments */
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_include_path)
	ZEND_PARSE_PARAMETERS_END();

	md.stream = php_stream_open_wrapper(filename, "rb",
			(use_include_path ? USE_PATH : 0) | REPORT_ERRORS,
			NULL);
	if (!md.stream)	{
		RETURN_FALSE;
	}

	array_init(return_value);

	tok_last = TOK_EOF;

	while (!done && (tok = php_next_meta_token(&md)) != TOK_EOF) {
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
					if (name) efree(name);
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
					if (value) efree(value);
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
				if (name) efree(name);
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
				if (value) efree(value);
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
				zend_str_tolower(name, strlen(name));
				if (have_content) {
					add_assoc_string(return_value, name, value);
				} else {
					add_assoc_string(return_value, name, "");
				}

				efree(name);
				if (value) efree(value);
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

	if (value) efree(value);
	if (name) efree(name);
	php_stream_close(md.stream);
}
/* }}} */

/* {{{ Read the entire file into a string */
PHP_FUNCTION(file_get_contents)
{
	char *filename;
	size_t filename_len;
	bool use_include_path = 0;
	php_stream *stream;
	zend_long offset = 0;
	zend_long maxlen;
	bool maxlen_is_null = 1;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;
	zend_string *contents;

	/* Parse arguments */
	ZEND_PARSE_PARAMETERS_START(1, 5)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_include_path)
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
		Z_PARAM_LONG(offset)
		Z_PARAM_LONG_OR_NULL(maxlen, maxlen_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (maxlen_is_null) {
		maxlen = (ssize_t) PHP_STREAM_COPY_ALL;
	} else if (maxlen < 0) {
		zend_argument_value_error(5, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	context = php_stream_context_from_zval(zcontext, 0);

	stream = php_stream_open_wrapper_ex(filename, "rb",
				(use_include_path ? USE_PATH : 0) | REPORT_ERRORS,
				NULL, context);
	if (!stream) {
		RETURN_FALSE;
	}

	/* disabling the read buffer allows doing the whole transfer
	   in just one read() system call */
	if (php_stream_is(stream, PHP_STREAM_IS_STDIO)) {
		php_stream_set_option(stream, PHP_STREAM_OPTION_READ_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
	}

	if (offset != 0 && php_stream_seek(stream, offset, ((offset > 0) ? SEEK_SET : SEEK_END)) < 0) {
		php_error_docref(NULL, E_WARNING, "Failed to seek to position " ZEND_LONG_FMT " in the stream", offset);
		php_stream_close(stream);
		RETURN_FALSE;
	}

	if ((contents = php_stream_copy_to_mem(stream, maxlen, 0)) != NULL) {
		RETVAL_STR(contents);
	} else {
		RETVAL_EMPTY_STRING();
	}

	php_stream_close(stream);
}
/* }}} */

/* {{{ Write/Create a file with contents data and return the number of bytes written */
PHP_FUNCTION(file_put_contents)
{
	php_stream *stream;
	char *filename;
	size_t filename_len;
	zval *data;
	ssize_t numbytes = 0;
	zend_long flags = 0;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;
	php_stream *srcstream = NULL;
	char mode[3] = "wb";

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_ZVAL(data)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(data) == IS_RESOURCE) {
		php_stream_from_zval(srcstream, data);
	}

	context = php_stream_context_from_zval(zcontext, flags & PHP_FILE_NO_DEFAULT_CONTEXT);

	if (flags & PHP_FILE_APPEND) {
		mode[0] = 'a';
	} else if (flags & LOCK_EX) {
		/* check to make sure we are dealing with a regular file */
		if (php_memnstr(filename, "://", sizeof("://") - 1, filename + filename_len)) {
			if (strncasecmp(filename, "file://", sizeof("file://") - 1)) {
				php_error_docref(NULL, E_WARNING, "Exclusive locks may only be set for regular files");
				RETURN_FALSE;
			}
		}
		mode[0] = 'c';
	}
	mode[2] = '\0';

	stream = php_stream_open_wrapper_ex(filename, mode, ((flags & PHP_FILE_USE_INCLUDE_PATH) ? USE_PATH : 0) | REPORT_ERRORS, NULL, context);
	if (stream == NULL) {
		RETURN_FALSE;
	}

	if ((flags & LOCK_EX) && (!php_stream_supports_lock(stream) || php_stream_lock(stream, LOCK_EX))) {
		php_stream_close(stream);
		php_error_docref(NULL, E_WARNING, "Exclusive locks are not supported for this stream");
		RETURN_FALSE;
	}

	if (mode[0] == 'c') {
		php_stream_truncate_set_size(stream, 0);
	}

	switch (Z_TYPE_P(data)) {
		case IS_RESOURCE: {
			size_t len;
			if (php_stream_copy_to_stream_ex(srcstream, stream, PHP_STREAM_COPY_ALL, &len) != SUCCESS) {
				numbytes = -1;
			} else {
				if (len > ZEND_LONG_MAX) {
					php_error_docref(NULL, E_WARNING, "content truncated from %zu to " ZEND_LONG_FMT " bytes", len, ZEND_LONG_MAX);
					len = ZEND_LONG_MAX;
				}
				numbytes = len;
			}
			break;
		}
		case IS_NULL:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_FALSE:
		case IS_TRUE:
			convert_to_string(data);
			ZEND_FALLTHROUGH;
		case IS_STRING:
			if (Z_STRLEN_P(data)) {
				numbytes = php_stream_write(stream, Z_STRVAL_P(data), Z_STRLEN_P(data));
				if (numbytes != -1 && numbytes != Z_STRLEN_P(data)) {
					php_error_docref(NULL, E_WARNING, "Only %zd of %zd bytes written, possibly out of free disk space", numbytes, Z_STRLEN_P(data));
					numbytes = -1;
				}
			}
			break;

		case IS_ARRAY:
			if (zend_hash_num_elements(Z_ARRVAL_P(data))) {
				ssize_t bytes_written;
				zval *tmp;

				ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(data), tmp) {
					zend_string *t;
					zend_string *str = zval_get_tmp_string(tmp, &t);
					if (ZSTR_LEN(str)) {
						numbytes += ZSTR_LEN(str);
						bytes_written = php_stream_write(stream, ZSTR_VAL(str), ZSTR_LEN(str));
						if (bytes_written != ZSTR_LEN(str)) {
							php_error_docref(NULL, E_WARNING, "Failed to write %zd bytes to %s", ZSTR_LEN(str), filename);
							zend_tmp_string_release(t);
							numbytes = -1;
							break;
						}
					}
					zend_tmp_string_release(t);
				} ZEND_HASH_FOREACH_END();
			}
			break;

		case IS_OBJECT:
			if (Z_OBJ_HT_P(data) != NULL) {
				zval out;

				if (zend_std_cast_object_tostring(Z_OBJ_P(data), &out, IS_STRING) == SUCCESS) {
					numbytes = php_stream_write(stream, Z_STRVAL(out), Z_STRLEN(out));
					if (numbytes != -1 && numbytes != Z_STRLEN(out)) {
						php_error_docref(NULL, E_WARNING, "Only %zd of %zd bytes written, possibly out of free disk space", numbytes, Z_STRLEN(out));
						numbytes = -1;
					}
					zval_ptr_dtor_str(&out);
					break;
				}
			}
			ZEND_FALLTHROUGH;
		default:
			numbytes = -1;
			break;
	}
	php_stream_close(stream);

	if (numbytes < 0) {
		RETURN_FALSE;
	}

	RETURN_LONG(numbytes);
}
/* }}} */

#define PHP_FILE_BUF_SIZE	80

/* {{{ Read entire file into an array */
PHP_FUNCTION(file)
{
	char *filename;
	size_t filename_len;
	char *p, *s, *e;
	int i = 0;
	char eol_marker = '\n';
	zend_long flags = 0;
	bool use_include_path;
	bool include_new_line;
	bool skip_blank_lines;
	php_stream *stream;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;
	zend_string *target_buf;

	/* Parse arguments */
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	if ((flags & ~(PHP_FILE_USE_INCLUDE_PATH | PHP_FILE_IGNORE_NEW_LINES | PHP_FILE_SKIP_EMPTY_LINES | PHP_FILE_NO_DEFAULT_CONTEXT)) != 0) {
		zend_argument_value_error(2, "must be a valid flag value");
		RETURN_THROWS();
	}

	use_include_path = flags & PHP_FILE_USE_INCLUDE_PATH;
	include_new_line = !(flags & PHP_FILE_IGNORE_NEW_LINES);
	skip_blank_lines = flags & PHP_FILE_SKIP_EMPTY_LINES;

	context = php_stream_context_from_zval(zcontext, flags & PHP_FILE_NO_DEFAULT_CONTEXT);

	stream = php_stream_open_wrapper_ex(filename, "rb", (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, context);
	if (!stream) {
		RETURN_FALSE;
	}

	/* Initialize return array */
	array_init(return_value);

	if ((target_buf = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0)) != NULL) {
		s = ZSTR_VAL(target_buf);
		e = ZSTR_VAL(target_buf) + ZSTR_LEN(target_buf);

		if (!(p = (char*)php_stream_locate_eol(stream, target_buf))) {
			p = e;
			goto parse_eol;
		}

		if (stream->flags & PHP_STREAM_FLAG_EOL_MAC) {
			eol_marker = '\r';
		}

		/* for performance reasons the code is duplicated, so that the if (include_new_line)
		 * will not need to be done for every single line in the file. */
		if (include_new_line) {
			do {
				p++;
parse_eol:
				add_index_stringl(return_value, i++, s, p-s);
				s = p;
			} while ((p = memchr(p, eol_marker, (e-p))));
		} else {
			do {
				int windows_eol = 0;
				if (p != ZSTR_VAL(target_buf) && eol_marker == '\n' && *(p - 1) == '\r') {
					windows_eol++;
				}
				if (skip_blank_lines && !(p-s-windows_eol)) {
					s = ++p;
					continue;
				}
				add_index_stringl(return_value, i++, s, p-s-windows_eol);
				s = ++p;
			} while ((p = memchr(p, eol_marker, (e-p))));
		}

		/* handle any leftovers of files without new lines */
		if (s != e) {
			p = e;
			goto parse_eol;
		}
	}

	if (target_buf) {
		zend_string_free(target_buf);
	}
	php_stream_close(stream);
}
/* }}} */

/* {{{ Create a unique filename in a directory */
PHP_FUNCTION(tempnam)
{
	char *dir, *prefix;
	size_t dir_len, prefix_len;
	zend_string *opened_path;
	int fd;
	zend_string *p;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_PATH(dir, dir_len)
		Z_PARAM_PATH(prefix, prefix_len)
	ZEND_PARSE_PARAMETERS_END();

	p = php_basename(prefix, prefix_len, NULL, 0);
	if (ZSTR_LEN(p) >= 64) {
		ZSTR_VAL(p)[63] = '\0';
	}

	RETVAL_FALSE;

	if ((fd = php_open_temporary_fd_ex(dir, ZSTR_VAL(p), &opened_path, PHP_TMP_FILE_OPEN_BASEDIR_CHECK_ALWAYS)) >= 0) {
		close(fd);
		RETVAL_STR(opened_path);
	}
	zend_string_release_ex(p, 0);
}
/* }}} */

/* {{{ Create a temporary file that will be deleted automatically after use */
PHP_FUNCTION(tmpfile)
{
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_NONE();

	stream = php_stream_fopen_tmpfile();

	if (stream) {
		php_stream_to_zval(stream, return_value);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Open a file or a URL and return a file pointer */
PHP_FUNCTION(fopen)
{
	char *filename, *mode;
	size_t filename_len, mode_len;
	bool use_include_path = 0;
	zval *zcontext = NULL;
	php_stream *stream;
	php_stream_context *context = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_STRING(mode, mode_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_include_path)
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	context = php_stream_context_from_zval(zcontext, 0);

	stream = php_stream_open_wrapper_ex(filename, mode, (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, context);

	if (stream == NULL) {
		RETURN_FALSE;
	}

	php_stream_to_zval(stream, return_value);
}
/* }}} */

/* {{{ Close an open file pointer */
PHPAPI PHP_FUNCTION(fclose)
{
	zval *res;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	if ((stream->flags & PHP_STREAM_FLAG_NO_FCLOSE) != 0) {
		php_error_docref(NULL, E_WARNING, ZEND_LONG_FMT " is not a valid stream resource", stream->res->handle);
		RETURN_FALSE;
	}

	php_stream_free(stream,
		PHP_STREAM_FREE_KEEP_RSRC |
		(stream->is_persistent ? PHP_STREAM_FREE_CLOSE_PERSISTENT : PHP_STREAM_FREE_CLOSE));

	RETURN_TRUE;
}
/* }}} */

/* {{{ Execute a command and open either a read or a write pipe to it */
PHP_FUNCTION(popen)
{
	char *command, *mode;
	size_t command_len, mode_len;
	FILE *fp;
	php_stream *stream;
	char *posix_mode;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_PATH(command, command_len)
		Z_PARAM_STRING(mode, mode_len)
	ZEND_PARSE_PARAMETERS_END();

	posix_mode = estrndup(mode, mode_len);
#ifndef PHP_WIN32
	{
		char *z = memchr(posix_mode, 'b', mode_len);
		if (z) {
			memmove(z, z + 1, mode_len - (z - posix_mode));
			mode_len--;
		}
	}
#endif

	/* Musl only partially validates the mode. Manually check it to ensure consistent behavior. */
	if (mode_len > 2 ||
		(mode_len == 1 && (*posix_mode != 'r' && *posix_mode != 'w')) ||
		(mode_len == 2 && (memcmp(posix_mode, "rb", 2) && memcmp(posix_mode, "wb", 2)))
	) {
		zend_argument_value_error(2, "must be one of \"r\", \"rb\", \"w\", or \"wb\"");
		efree(posix_mode);
		RETURN_THROWS();
	}

	fp = VCWD_POPEN(command, posix_mode);
	if (!fp) {
		php_error_docref2(NULL, command, posix_mode, E_WARNING, "%s", strerror(errno));
		efree(posix_mode);
		RETURN_FALSE;
	}

	stream = php_stream_fopen_from_pipe(fp, mode);

	if (stream == NULL)	{
		php_error_docref2(NULL, command, mode, E_WARNING, "%s", strerror(errno));
		RETVAL_FALSE;
	} else {
		php_stream_to_zval(stream, return_value);
	}

	efree(posix_mode);
}
/* }}} */

/* {{{ Close a file pointer opened by popen() */
PHP_FUNCTION(pclose)
{
	zval *res;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	FG(pclose_wait) = 1;
	zend_list_close(stream->res);
	FG(pclose_wait) = 0;
	RETURN_LONG(FG(pclose_ret));
}
/* }}} */

/* {{{ Test for end-of-file on a file pointer */
PHPAPI PHP_FUNCTION(feof)
{
	zval *res;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	if (php_stream_eof(stream)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Get a line from file pointer */
PHPAPI PHP_FUNCTION(fgets)
{
	zval *res;
	zend_long len = 1024;
	bool len_is_null = 1;
	char *buf = NULL;
	size_t line_len = 0;
	zend_string *str;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_RESOURCE(res)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(len, len_is_null)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	if (len_is_null) {
		/* ask streams to give us a buffer of an appropriate size */
		buf = php_stream_get_line(stream, NULL, 0, &line_len);
		if (buf == NULL) {
			RETURN_FALSE;
		}
		// TODO: avoid reallocation ???
		RETVAL_STRINGL(buf, line_len);
		efree(buf);
	} else {
		if (len <= 0) {
			zend_argument_value_error(2, "must be greater than 0");
			RETURN_THROWS();
		}

		str = zend_string_alloc(len, 0);
		if (php_stream_get_line(stream, ZSTR_VAL(str), len, &line_len) == NULL) {
			zend_string_efree(str);
			RETURN_FALSE;
		}
		/* resize buffer if it's much larger than the result.
		 * Only needed if the user requested a buffer size. */
		if (line_len < (size_t)len / 2) {
			str = zend_string_truncate(str, line_len, 0);
		} else {
			ZSTR_LEN(str) = line_len;
		}
		RETURN_NEW_STR(str);
	}
}
/* }}} */

/* {{{ Get a character from file pointer */
PHPAPI PHP_FUNCTION(fgetc)
{
	zval *res;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	int result = php_stream_getc(stream);

	if (result == EOF) {
		RETVAL_FALSE;
	} else {
		RETURN_CHAR(result);
	}
}
/* }}} */

/* {{{ Implements a mostly ANSI compatible fscanf() */
PHP_FUNCTION(fscanf)
{
	int result, argc = 0;
	size_t format_len;
	zval *args = NULL;
	zval *file_handle;
	char *buf, *format;
	size_t len;
	void *what;

	ZEND_PARSE_PARAMETERS_START(2, -1)
		Z_PARAM_RESOURCE(file_handle)
		Z_PARAM_STRING(format, format_len)
		Z_PARAM_VARIADIC('*', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	what = zend_fetch_resource2(Z_RES_P(file_handle), "File-Handle", php_file_le_stream(), php_file_le_pstream());

	/* we can't do a ZEND_VERIFY_RESOURCE(what), otherwise we end up
	 * with a leak if we have an invalid filehandle. This needs changing
	 * if the code behind ZEND_VERIFY_RESOURCE changed. - cc */
	if (!what) {
		RETURN_THROWS();
	}

	buf = php_stream_get_line((php_stream *) what, NULL, 0, &len);
	if (buf == NULL) {
		RETURN_FALSE;
	}

	result = php_sscanf_internal(buf, format, argc, args, 0, return_value);

	efree(buf);

	if (SCAN_ERROR_WRONG_PARAM_COUNT == result) {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ Binary-safe file write */
PHPAPI PHP_FUNCTION(fwrite)
{
	zval *res;
	char *input;
	size_t inputlen;
	ssize_t ret;
	size_t num_bytes;
	zend_long maxlen = 0;
	bool maxlen_is_null = 1;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_RESOURCE(res)
		Z_PARAM_STRING(input, inputlen)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(maxlen, maxlen_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (maxlen_is_null) {
		num_bytes = inputlen;
	} else if (maxlen <= 0) {
		num_bytes = 0;
	} else {
		num_bytes = MIN((size_t) maxlen, inputlen);
	}

	if (!num_bytes) {
		RETURN_LONG(0);
	}

	PHP_STREAM_FROM_ZVAL(stream, res);

	ret = php_stream_write(stream, input, num_bytes);
	if (ret < 0) {
		RETURN_FALSE;
	}

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ Flushes output */
PHPAPI PHP_FUNCTION(fflush)
{
	zval *res;
	int ret;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	ret = php_stream_flush(stream);
	if (ret) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Rewind the position of a file pointer */
PHPAPI PHP_FUNCTION(rewind)
{
	zval *res;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	if (-1 == php_stream_rewind(stream)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Get file pointer's read/write position */
PHPAPI PHP_FUNCTION(ftell)
{
	zval *res;
	zend_long ret;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	ret = php_stream_tell(stream);
	if (ret == -1)	{
		RETURN_FALSE;
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ Seek on a file pointer */
PHPAPI PHP_FUNCTION(fseek)
{
	zval *res;
	zend_long offset, whence = SEEK_SET;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_RESOURCE(res)
		Z_PARAM_LONG(offset)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(whence)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	RETURN_LONG(php_stream_seek(stream, offset, (int) whence));
}
/* }}} */

/* {{{ php_mkdir */

/* DEPRECATED APIs: Use php_stream_mkdir() instead */
PHPAPI int php_mkdir_ex(const char *dir, zend_long mode, int options)
{
	int ret;

	if (php_check_open_basedir(dir)) {
		return -1;
	}

	if ((ret = VCWD_MKDIR(dir, (mode_t)mode)) < 0 && (options & REPORT_ERRORS)) {
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
	}

	return ret;
}

PHPAPI int php_mkdir(const char *dir, zend_long mode)
{
	return php_mkdir_ex(dir, mode, REPORT_ERRORS);
}
/* }}} */

/* {{{ Create a directory */
PHP_FUNCTION(mkdir)
{
	char *dir;
	size_t dir_len;
	zval *zcontext = NULL;
	zend_long mode = 0777;
	bool recursive = 0;
	php_stream_context *context;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_PATH(dir, dir_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(mode)
		Z_PARAM_BOOL(recursive)
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	context = php_stream_context_from_zval(zcontext, 0);

	RETURN_BOOL(php_stream_mkdir(dir, (int)mode, (recursive ? PHP_STREAM_MKDIR_RECURSIVE : 0) | REPORT_ERRORS, context));
}
/* }}} */

/* {{{ Remove a directory */
PHP_FUNCTION(rmdir)
{
	char *dir;
	size_t dir_len;
	zval *zcontext = NULL;
	php_stream_context *context;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(dir, dir_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	context = php_stream_context_from_zval(zcontext, 0);

	RETURN_BOOL(php_stream_rmdir(dir, REPORT_ERRORS, context));
}
/* }}} */

/* {{{ Output a file or a URL */
PHP_FUNCTION(readfile)
{
	char *filename;
	size_t filename_len;
	size_t size = 0;
	bool use_include_path = 0;
	zval *zcontext = NULL;
	php_stream *stream;
	php_stream_context *context = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_include_path)
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	context = php_stream_context_from_zval(zcontext, 0);

	stream = php_stream_open_wrapper_ex(filename, "rb", (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, context);
	if (stream) {
		size = php_stream_passthru(stream);
		php_stream_close(stream);
		RETURN_LONG(size);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Return or change the umask */
PHP_FUNCTION(umask)
{
	zend_long mask = 0;
	bool mask_is_null = 1;
	int oldumask;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(mask, mask_is_null)
	ZEND_PARSE_PARAMETERS_END();

	oldumask = umask(077);

	if (BG(umask) == -1) {
		BG(umask) = oldumask;
	}

	if (mask_is_null) {
		umask(oldumask);
	} else {
		umask((int) mask);
	}

	RETURN_LONG(oldumask);
}
/* }}} */

/* {{{ Output all remaining data from a file pointer */
PHPAPI PHP_FUNCTION(fpassthru)
{
	zval *res;
	size_t size;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	size = php_stream_passthru(stream);
	RETURN_LONG(size);
}
/* }}} */

/* {{{ Rename a file */
PHP_FUNCTION(rename)
{
	char *old_name, *new_name;
	size_t old_name_len, new_name_len;
	zval *zcontext = NULL;
	php_stream_wrapper *wrapper;
	php_stream_context *context;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_PATH(old_name, old_name_len)
		Z_PARAM_PATH(new_name, new_name_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	wrapper = php_stream_locate_url_wrapper(old_name, NULL, 0);

	if (!wrapper || !wrapper->wops) {
		php_error_docref(NULL, E_WARNING, "Unable to locate stream wrapper");
		RETURN_FALSE;
	}

	if (!wrapper->wops->rename) {
		php_error_docref(NULL, E_WARNING, "%s wrapper does not support renaming", wrapper->wops->label ? wrapper->wops->label : "Source");
		RETURN_FALSE;
	}

	if (wrapper != php_stream_locate_url_wrapper(new_name, NULL, 0)) {
		php_error_docref(NULL, E_WARNING, "Cannot rename a file across wrapper types");
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);

	RETURN_BOOL(wrapper->wops->rename(wrapper, old_name, new_name, 0, context));
}
/* }}} */

/* {{{ Delete a file */
PHP_FUNCTION(unlink)
{
	char *filename;
	size_t filename_len;
	php_stream_wrapper *wrapper;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	context = php_stream_context_from_zval(zcontext, 0);

	wrapper = php_stream_locate_url_wrapper(filename, NULL, 0);

	if (!wrapper || !wrapper->wops) {
		php_error_docref(NULL, E_WARNING, "Unable to locate stream wrapper");
		RETURN_FALSE;
	}

	if (!wrapper->wops->unlink) {
		php_error_docref(NULL, E_WARNING, "%s does not allow unlinking", wrapper->wops->label ? wrapper->wops->label : "Wrapper");
		RETURN_FALSE;
	}
	RETURN_BOOL(wrapper->wops->unlink(wrapper, filename, REPORT_ERRORS, context));
}
/* }}} */

PHP_FUNCTION(fsync)
{
	zval *res;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	if (!php_stream_sync_supported(stream)) {
		php_error_docref(NULL, E_WARNING, "Can't fsync this stream!");
		RETURN_FALSE;
	}

	RETURN_BOOL(php_stream_sync(stream, /* data_only */ 0) == 0);
}

PHP_FUNCTION(fdatasync)
{
	zval *res;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	if (!php_stream_sync_supported(stream)) {
		php_error_docref(NULL, E_WARNING, "Can't fsync this stream!");
		RETURN_FALSE;
	}

	RETURN_BOOL(php_stream_sync(stream, /* data_only */ 1) == 0);
}

/* {{{ Truncate file to 'size' length */
PHP_FUNCTION(ftruncate)
{
	zval *fp;
	zend_long size;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(fp)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (size < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	PHP_STREAM_FROM_ZVAL(stream, fp);

	if (!php_stream_truncate_supported(stream)) {
		php_error_docref(NULL, E_WARNING, "Can't truncate this stream!");
		RETURN_FALSE;
	}

	RETURN_BOOL(0 == php_stream_truncate_set_size(stream, size));
}
/* }}} */
PHPAPI void php_fstat(php_stream *stream, zval *return_value)
{
	php_stream_statbuf stat_ssb;
	zval stat_dev, stat_ino, stat_mode, stat_nlink, stat_uid, stat_gid, stat_rdev,
		 stat_size, stat_atime, stat_mtime, stat_ctime, stat_blksize, stat_blocks;
	char *stat_sb_names[13] = {
		"dev", "ino", "mode", "nlink", "uid", "gid", "rdev",
		"size", "atime", "mtime", "ctime", "blksize", "blocks"
	};

	if (php_stream_stat(stream, &stat_ssb)) {
		RETURN_FALSE;
	}

	array_init(return_value);

	ZVAL_LONG(&stat_dev, stat_ssb.sb.st_dev);
	ZVAL_LONG(&stat_ino, stat_ssb.sb.st_ino);
	ZVAL_LONG(&stat_mode, stat_ssb.sb.st_mode);
	ZVAL_LONG(&stat_nlink, stat_ssb.sb.st_nlink);
	ZVAL_LONG(&stat_uid, stat_ssb.sb.st_uid);
	ZVAL_LONG(&stat_gid, stat_ssb.sb.st_gid);
#ifdef HAVE_STRUCT_STAT_ST_RDEV
	ZVAL_LONG(&stat_rdev, stat_ssb.sb.st_rdev);
#else
	ZVAL_LONG(&stat_rdev, -1);
#endif
	ZVAL_LONG(&stat_size, stat_ssb.sb.st_size);
	ZVAL_LONG(&stat_atime, stat_ssb.sb.st_atime);
	ZVAL_LONG(&stat_mtime, stat_ssb.sb.st_mtime);
	ZVAL_LONG(&stat_ctime, stat_ssb.sb.st_ctime);
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
	ZVAL_LONG(&stat_blksize, stat_ssb.sb.st_blksize);
#else
	ZVAL_LONG(&stat_blksize,-1);
#endif
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
	ZVAL_LONG(&stat_blocks, stat_ssb.sb.st_blocks);
#else
	ZVAL_LONG(&stat_blocks,-1);
#endif
	/* Store numeric indexes in proper order */
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_dev);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_ino);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_mode);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_nlink);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_uid);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_gid);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_rdev);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_size);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_atime);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_mtime);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_ctime);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_blksize);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &stat_blocks);

	/* Store string indexes referencing the same zval*/
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[0], strlen(stat_sb_names[0]), &stat_dev);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[1], strlen(stat_sb_names[1]), &stat_ino);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[2], strlen(stat_sb_names[2]), &stat_mode);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[3], strlen(stat_sb_names[3]), &stat_nlink);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[4], strlen(stat_sb_names[4]), &stat_uid);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[5], strlen(stat_sb_names[5]), &stat_gid);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[6], strlen(stat_sb_names[6]), &stat_rdev);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[7], strlen(stat_sb_names[7]), &stat_size);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[8], strlen(stat_sb_names[8]), &stat_atime);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[9], strlen(stat_sb_names[9]), &stat_mtime);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[10], strlen(stat_sb_names[10]), &stat_ctime);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[11], strlen(stat_sb_names[11]), &stat_blksize);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), stat_sb_names[12], strlen(stat_sb_names[12]), &stat_blocks);
}

/* {{{ Stat() on a filehandle */
PHP_FUNCTION(fstat)
{
	zval *fp;
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(fp)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, fp);

	php_fstat(stream, return_value);
}
/* }}} */

/* {{{ Copy a file */
PHP_FUNCTION(copy)
{
	char *source, *target;
	size_t source_len, target_len;
	zval *zcontext = NULL;
	php_stream_context *context;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_PATH(source, source_len)
		Z_PARAM_PATH(target, target_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_RESOURCE_OR_NULL(zcontext)
	ZEND_PARSE_PARAMETERS_END();

	if (php_stream_locate_url_wrapper(source, NULL, 0) == &php_plain_files_wrapper && php_check_open_basedir(source)) {
		RETURN_FALSE;
	}

	context = php_stream_context_from_zval(zcontext, 0);

	if (php_copy_file_ctx(source, target, 0, context) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_copy_file */
PHPAPI int php_copy_file(const char *src, const char *dest)
{
	return php_copy_file_ctx(src, dest, 0, NULL);
}
/* }}} */

/* {{{ php_copy_file_ex */
PHPAPI int php_copy_file_ex(const char *src, const char *dest, int src_flg)
{
	return php_copy_file_ctx(src, dest, src_flg, NULL);
}
/* }}} */

/* {{{ php_copy_file_ctx */
PHPAPI int php_copy_file_ctx(const char *src, const char *dest, int src_flg, php_stream_context *ctx)
{
	php_stream *srcstream = NULL, *deststream = NULL;
	int ret = FAILURE;
	php_stream_statbuf src_s, dest_s;
	int src_stat_flags = (src_flg & STREAM_DISABLE_OPEN_BASEDIR) ? PHP_STREAM_URL_STAT_IGNORE_OPEN_BASEDIR : 0;

	switch (php_stream_stat_path_ex(src, src_stat_flags, &src_s, ctx)) {
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
		php_error_docref(NULL, E_WARNING, "The first argument to copy() function cannot be a directory");
		return FAILURE;
	}

	switch (php_stream_stat_path_ex(dest, PHP_STREAM_URL_STAT_QUIET, &dest_s, ctx)) {
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
		php_error_docref(NULL, E_WARNING, "The second argument to copy() function cannot be a directory");
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

		if ((sp = expand_filepath(src, NULL)) == NULL) {
			return ret;
		}
		if ((dp = expand_filepath(dest, NULL)) == NULL) {
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

	srcstream = php_stream_open_wrapper_ex(src, "rb", src_flg | REPORT_ERRORS, NULL, ctx);

	if (!srcstream) {
		return ret;
	}

	deststream = php_stream_open_wrapper_ex(dest, "wb", REPORT_ERRORS, NULL, ctx);

	if (srcstream && deststream) {
		ret = php_stream_copy_to_stream_ex(srcstream, deststream, PHP_STREAM_COPY_ALL, NULL);
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

/* {{{ Binary-safe file read */
PHPAPI PHP_FUNCTION(fread)
{
	zval *res;
	zend_long len;
	php_stream *stream;
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(res)
		Z_PARAM_LONG(len)
	ZEND_PARSE_PARAMETERS_END();

	PHP_STREAM_FROM_ZVAL(stream, res);

	if (len <= 0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	str = php_stream_read_to_str(stream, len);
	if (!str) {
		zval_ptr_dtor_str(return_value);
		RETURN_FALSE;
	}

	RETURN_STR(str);
}
/* }}} */

static const char *php_fgetcsv_lookup_trailing_spaces(const char *ptr, size_t len) /* {{{ */
{
	int inc_len;
	unsigned char last_chars[2] = { 0, 0 };

	while (len > 0) {
		inc_len = (*ptr == '\0' ? 1 : php_mblen(ptr, len));
		switch (inc_len) {
			case -2:
			case -1:
				inc_len = 1;
				php_mb_reset();
				break;
			case 0:
				goto quit_loop;
			case 1:
			default:
				last_chars[0] = last_chars[1];
				last_chars[1] = *ptr;
				break;
		}
		ptr += inc_len;
		len -= inc_len;
	}
quit_loop:
	switch (last_chars[1]) {
		case '\n':
			if (last_chars[0] == '\r') {
				return ptr - 2;
			}
			ZEND_FALLTHROUGH;
		case '\r':
			return ptr - 1;
	}
	return ptr;
}
/* }}} */

#define FPUTCSV_FLD_CHK(c) memchr(ZSTR_VAL(field_str), c, ZSTR_LEN(field_str))

/* {{{ Format line as CSV and write to file pointer */
PHP_FUNCTION(fputcsv)
{
	char delimiter = ',';					/* allow this to be set as parameter */
	char enclosure = '"';					/* allow this to be set as parameter */
	int escape_char = (unsigned char) '\\';	/* allow this to be set as parameter */
	php_stream *stream;
	zval *fp = NULL, *fields = NULL;
	ssize_t ret;
	char *delimiter_str = NULL, *enclosure_str = NULL, *escape_str = NULL;
	size_t delimiter_str_len = 0, enclosure_str_len = 0, escape_str_len = 0;
	zend_string *eol_str = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 6)
		Z_PARAM_RESOURCE(fp)
		Z_PARAM_ARRAY(fields)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(delimiter_str, delimiter_str_len)
		Z_PARAM_STRING(enclosure_str, enclosure_str_len)
		Z_PARAM_STRING(escape_str, escape_str_len)
		Z_PARAM_STR_OR_NULL(eol_str)
	ZEND_PARSE_PARAMETERS_END();

	if (delimiter_str != NULL) {
		/* Make sure that there is at least one character in string */
		if (delimiter_str_len != 1) {
			zend_argument_value_error(3, "must be a single character");
			RETURN_THROWS();
		}

		/* use first character from string */
		delimiter = *delimiter_str;
	}

	if (enclosure_str != NULL) {
		if (enclosure_str_len != 1) {
			zend_argument_value_error(4, "must be a single character");
			RETURN_THROWS();
		}
		/* use first character from string */
		enclosure = *enclosure_str;
	}

	if (escape_str != NULL) {
		if (escape_str_len > 1) {
			zend_argument_value_error(5, "must be empty or a single character");
			RETURN_THROWS();
		}
		if (escape_str_len < 1) {
			escape_char = PHP_CSV_NO_ESCAPE;
		} else {
			/* use first character from string */
			escape_char = (unsigned char) *escape_str;
		}
	}

	PHP_STREAM_FROM_ZVAL(stream, fp);

	ret = php_fputcsv(stream, fields, delimiter, enclosure, escape_char, eol_str);
	if (ret < 0) {
		RETURN_FALSE;
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ PHPAPI size_t php_fputcsv(php_stream *stream, zval *fields, char delimiter, char enclosure, int escape_char, zend_string *eol_str) */
PHPAPI ssize_t php_fputcsv(php_stream *stream, zval *fields, char delimiter, char enclosure, int escape_char, zend_string *eol_str)
{
	uint32_t count, i = 0;
	size_t ret;
	zval *field_tmp;
	smart_str csvline = {0};

	ZEND_ASSERT((escape_char >= 0 && escape_char <= UCHAR_MAX) || escape_char == PHP_CSV_NO_ESCAPE);
	count = zend_hash_num_elements(Z_ARRVAL_P(fields));
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(fields), field_tmp) {
		zend_string *tmp_field_str;
		zend_string *field_str = zval_get_tmp_string(field_tmp, &tmp_field_str);

		/* enclose a field that contains a delimiter, an enclosure character, or a newline */
		if (FPUTCSV_FLD_CHK(delimiter) ||
			FPUTCSV_FLD_CHK(enclosure) ||
			(escape_char != PHP_CSV_NO_ESCAPE && FPUTCSV_FLD_CHK(escape_char)) ||
			FPUTCSV_FLD_CHK('\n') ||
			FPUTCSV_FLD_CHK('\r') ||
			FPUTCSV_FLD_CHK('\t') ||
			FPUTCSV_FLD_CHK(' ')
		) {
			char *ch = ZSTR_VAL(field_str);
			char *end = ch + ZSTR_LEN(field_str);
			int escaped = 0;

			smart_str_appendc(&csvline, enclosure);
			while (ch < end) {
				if (escape_char != PHP_CSV_NO_ESCAPE && *ch == escape_char) {
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
			smart_str_append(&csvline, field_str);
		}

		if (++i != count) {
			smart_str_appendl(&csvline, &delimiter, 1);
		}
		zend_tmp_string_release(tmp_field_str);
	} ZEND_HASH_FOREACH_END();

	if (eol_str) {
		smart_str_append(&csvline, eol_str);
	} else {
		smart_str_appendc(&csvline, '\n');
	}
	smart_str_0(&csvline);

	ret = php_stream_write(stream, ZSTR_VAL(csvline.s), ZSTR_LEN(csvline.s));

	smart_str_free(&csvline);

	return ret;
}
/* }}} */

/* {{{ Get line from file pointer and parse for CSV fields */
PHP_FUNCTION(fgetcsv)
{
	char delimiter = ',';	/* allow this to be set as parameter */
	char enclosure = '"';	/* allow this to be set as parameter */
	int escape = (unsigned char) '\\';

	zend_long len = 0;
	size_t buf_len;
	char *buf;
	php_stream *stream;

	{
		zval *fd;
		bool len_is_null = 1;
		char *delimiter_str = NULL;
		size_t delimiter_str_len = 0;
		char *enclosure_str = NULL;
		size_t enclosure_str_len = 0;
		char *escape_str = NULL;
		size_t escape_str_len = 0;

		ZEND_PARSE_PARAMETERS_START(1, 5)
			Z_PARAM_RESOURCE(fd)
			Z_PARAM_OPTIONAL
			Z_PARAM_LONG_OR_NULL(len, len_is_null)
			Z_PARAM_STRING(delimiter_str, delimiter_str_len)
			Z_PARAM_STRING(enclosure_str, enclosure_str_len)
			Z_PARAM_STRING(escape_str, escape_str_len)
		ZEND_PARSE_PARAMETERS_END();

		if (delimiter_str != NULL) {
			/* Make sure that there is at least one character in string */
			if (delimiter_str_len != 1) {
				zend_argument_value_error(3, "must be a single character");
				RETURN_THROWS();
			}

			/* use first character from string */
			delimiter = delimiter_str[0];
		}

		if (enclosure_str != NULL) {
			if (enclosure_str_len != 1) {
				zend_argument_value_error(4, "must be a single character");
				RETURN_THROWS();
			}

			/* use first character from string */
			enclosure = enclosure_str[0];
		}

		if (escape_str != NULL) {
			if (escape_str_len > 1) {
				zend_argument_value_error(5, "must be empty or a single character");
				RETURN_THROWS();
			}

			if (escape_str_len < 1) {
				escape = PHP_CSV_NO_ESCAPE;
			} else {
				escape = (unsigned char) escape_str[0];
			}
		}

		if (len_is_null || len == 0) {
			len = -1;
		} else if (len < 0) {
			zend_argument_value_error(2, "must be a greater than or equal to 0");
			RETURN_THROWS();
		}

		PHP_STREAM_FROM_ZVAL(stream, fd);
	}

	if (len < 0) {
		if ((buf = php_stream_get_line(stream, NULL, 0, &buf_len)) == NULL) {
			RETURN_FALSE;
		}
	} else {
		buf = emalloc(len + 1);
		if (php_stream_get_line(stream, buf, len + 1, &buf_len) == NULL) {
			efree(buf);
			RETURN_FALSE;
		}
	}

	HashTable *values = php_fgetcsv(stream, delimiter, enclosure, escape, buf_len, buf);
	if (values == NULL) {
		values = php_bc_fgetcsv_empty_line();
	}
	RETURN_ARR(values);
}
/* }}} */

PHPAPI HashTable *php_bc_fgetcsv_empty_line(void)
{
	HashTable *values = zend_new_array(1);
	zval tmp;
	ZVAL_NULL(&tmp);
	zend_hash_next_index_insert(values, &tmp);
	return values;
}

PHPAPI HashTable *php_fgetcsv(php_stream *stream, char delimiter, char enclosure, int escape_char, size_t buf_len, char *buf) /* {{{ */
{
	char *temp, *bptr, *line_end, *limit;
	size_t temp_len, line_end_len;
	int inc_len;
	bool first_field = true;

	ZEND_ASSERT((escape_char >= 0 && escape_char <= UCHAR_MAX) || escape_char == PHP_CSV_NO_ESCAPE);

	/* initialize internal state */
	php_mb_reset();

	/* Now into new section that parses buf for delimiter/enclosure fields */

	/* Strip trailing space from buf, saving end of line in case required for enclosure field */

	bptr = buf;
	line_end = limit = (char *)php_fgetcsv_lookup_trailing_spaces(buf, buf_len);
	line_end_len = buf_len - (size_t)(limit - buf);

	/* reserve workspace for building each individual field */
	temp_len = buf_len;
	temp = emalloc(temp_len + line_end_len + 1);

	/* Initialize values HashTable */
	HashTable *values = zend_new_array(0);

	/* Main loop to read CSV fields */
	/* NB this routine will return NULL for a blank line */
	do {
		char *comp_end, *hunk_begin;
		char *tptr = temp;

		inc_len = (bptr < limit ? (*bptr == '\0' ? 1 : php_mblen(bptr, limit - bptr)): 0);
		if (inc_len == 1) {
			char *tmp = bptr;
			while ((*tmp != delimiter) && isspace((int)*(unsigned char *)tmp)) {
				tmp++;
			}
			if (*tmp == enclosure && tmp < limit) {
				bptr = tmp;
			}
		}

		if (first_field && bptr == line_end) {
			zend_array_destroy(values);
			values = NULL;
			break;
		}
		first_field = false;
		/* 2. Read field, leaving bptr pointing at start of next field */
		if (inc_len != 0 && *bptr == enclosure) {
			int state = 0;

			bptr++;	/* move on to first character in field */
			hunk_begin = bptr;

			/* 2A. handle enclosure delimited field */
			for (;;) {
				switch (inc_len) {
					case 0:
						switch (state) {
							case 2:
								memcpy(tptr, hunk_begin, bptr - hunk_begin - 1);
								tptr += (bptr - hunk_begin - 1);
								hunk_begin = bptr;
								goto quit_loop_2;

							case 1:
								memcpy(tptr, hunk_begin, bptr - hunk_begin);
								tptr += (bptr - hunk_begin);
								hunk_begin = bptr;
								ZEND_FALLTHROUGH;

							case 0: {
								if (hunk_begin != line_end) {
									memcpy(tptr, hunk_begin, bptr - hunk_begin);
									tptr += (bptr - hunk_begin);
									hunk_begin = bptr;
								}

								/* add the embedded line end to the field */
								memcpy(tptr, line_end, line_end_len);
								tptr += line_end_len;

								/* nothing can be fetched if stream is NULL (e.g. str_getcsv()) */
								if (stream == NULL) {
									/* the enclosure is unterminated */
									if (bptr > limit) {
										/* if the line ends with enclosure, we need to go back by
										 * one character so the \0 character is not copied. */
										--bptr;
									}
									goto quit_loop_2;
								}

								size_t new_len;
								char *new_buf = php_stream_get_line(stream, NULL, 0, &new_len);
								if (!new_buf) {
									/* we've got an unterminated enclosure,
									 * assign all the data from the start of
									 * the enclosure to end of data to the
									 * last element */
									if (bptr > limit) {
										/* if the line ends with enclosure, we need to go back by
										 * one character so the \0 character is not copied. */
										--bptr;
									}
									goto quit_loop_2;
								}

								temp_len += new_len;
								char *new_temp = erealloc(temp, temp_len);
								tptr = new_temp + (size_t)(tptr - temp);
								temp = new_temp;

								efree(buf);
								buf_len = new_len;
								bptr = buf = new_buf;
								hunk_begin = buf;

								line_end = limit = (char *)php_fgetcsv_lookup_trailing_spaces(buf, buf_len);
								line_end_len = buf_len - (size_t)(limit - buf);

								state = 0;
							} break;
						}
						break;

					case -2:
					case -1:
						php_mb_reset();
						ZEND_FALLTHROUGH;
					case 1:
						/* we need to determine if the enclosure is
						 * 'real' or is it escaped */
						switch (state) {
							case 1: /* escaped */
								bptr++;
								state = 0;
								break;
							case 2: /* embedded enclosure ? let's check it */
								if (*bptr != enclosure) {
									/* real enclosure */
									memcpy(tptr, hunk_begin, bptr - hunk_begin - 1);
									tptr += (bptr - hunk_begin - 1);
									hunk_begin = bptr;
									goto quit_loop_2;
								}
								memcpy(tptr, hunk_begin, bptr - hunk_begin);
								tptr += (bptr - hunk_begin);
								bptr++;
								hunk_begin = bptr;
								state = 0;
								break;
							default:
								if (*bptr == enclosure) {
									state = 2;
								} else if (escape_char != PHP_CSV_NO_ESCAPE && *bptr == escape_char) {
									state = 1;
								}
								bptr++;
								break;
						}
						break;

					default:
						switch (state) {
							case 2:
								/* real enclosure */
								memcpy(tptr, hunk_begin, bptr - hunk_begin - 1);
								tptr += (bptr - hunk_begin - 1);
								hunk_begin = bptr;
								goto quit_loop_2;
							case 1:
								bptr += inc_len;
								memcpy(tptr, hunk_begin, bptr - hunk_begin);
								tptr += (bptr - hunk_begin);
								hunk_begin = bptr;
								state = 0;
								break;
							default:
								bptr += inc_len;
								break;
						}
						break;
				}
				inc_len = (bptr < limit ? (*bptr == '\0' ? 1 : php_mblen(bptr, limit - bptr)): 0);
			}

		quit_loop_2:
			/* look up for a delimiter */
			for (;;) {
				switch (inc_len) {
					case 0:
						goto quit_loop_3;

					case -2:
					case -1:
						inc_len = 1;
						php_mb_reset();
						ZEND_FALLTHROUGH;
					case 1:
						if (*bptr == delimiter) {
							goto quit_loop_3;
						}
						break;
					default:
						break;
				}
				bptr += inc_len;
				inc_len = (bptr < limit ? (*bptr == '\0' ? 1 : php_mblen(bptr, limit - bptr)): 0);
			}

		quit_loop_3:
			memcpy(tptr, hunk_begin, bptr - hunk_begin);
			tptr += (bptr - hunk_begin);
			bptr += inc_len;
			comp_end = tptr;
		} else {
			/* 2B. Handle non-enclosure field */

			hunk_begin = bptr;

			for (;;) {
				switch (inc_len) {
					case 0:
						goto quit_loop_4;
					case -2:
					case -1:
						inc_len = 1;
						php_mb_reset();
						ZEND_FALLTHROUGH;
					case 1:
						if (*bptr == delimiter) {
							goto quit_loop_4;
						}
						break;
					default:
						break;
				}
				bptr += inc_len;
				inc_len = (bptr < limit ? (*bptr == '\0' ? 1 : php_mblen(bptr, limit - bptr)): 0);
			}
		quit_loop_4:
			memcpy(tptr, hunk_begin, bptr - hunk_begin);
			tptr += (bptr - hunk_begin);

			comp_end = (char *)php_fgetcsv_lookup_trailing_spaces(temp, tptr - temp);
			if (*bptr == delimiter) {
				bptr++;
			}
		}

		/* 3. Now pass our field back to php */
		*comp_end = '\0';

		zval z_tmp;
		ZVAL_STRINGL(&z_tmp, temp, comp_end - temp);
		zend_hash_next_index_insert(values, &z_tmp);
	} while (inc_len > 0);

	efree(temp);
	if (stream) {
		efree(buf);
	}

	return values;
}
/* }}} */

/* {{{ Return the resolved path */
PHP_FUNCTION(realpath)
{
	char *filename;
	size_t filename_len;
	char resolved_path_buff[MAXPATHLEN];

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(filename, filename_len)
	ZEND_PARSE_PARAMETERS_END();

	if (VCWD_REALPATH(filename, resolved_path_buff)) {
		if (php_check_open_basedir(resolved_path_buff)) {
			RETURN_FALSE;
		}

#ifdef ZTS
		if (VCWD_ACCESS(resolved_path_buff, F_OK)) {
			RETURN_FALSE;
		}
#endif
		RETURN_STRING(resolved_path_buff);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* See http://www.w3.org/TR/html4/intro/sgmltut.html#h-3.2.2 */
#define PHP_META_HTML401_CHARS "-_.:"

/* {{{ php_next_meta_token
   Tokenizes an HTML file for get_meta_tags */
php_meta_tags_token php_next_meta_token(php_meta_tags_data *md)
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
					/* Was just an apostrophe */
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
/* {{{ Match filename against pattern */
PHP_FUNCTION(fnmatch)
{
	char *pattern, *filename;
	size_t pattern_len, filename_len;
	zend_long flags = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_PATH(pattern, pattern_len)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	if (filename_len >= MAXPATHLEN) {
		php_error_docref(NULL, E_WARNING, "Filename exceeds the maximum allowed length of %d characters", MAXPATHLEN);
		RETURN_FALSE;
	}
	if (pattern_len >= MAXPATHLEN) {
		php_error_docref(NULL, E_WARNING, "Pattern exceeds the maximum allowed length of %d characters", MAXPATHLEN);
		RETURN_FALSE;
	}

	RETURN_BOOL( ! fnmatch( pattern, filename, (int)flags ));
}
/* }}} */
#endif

/* {{{ Returns directory path used for temporary files */
PHP_FUNCTION(sys_get_temp_dir)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STRING((char *)php_get_temporary_directory());
}
/* }}} */
