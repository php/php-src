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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with php 3.0 revision 1.30 1999-06-16 [ssb] */

#ifndef FILE_H
#define FILE_H

PHP_MINIT_FUNCTION(file);
PHP_MSHUTDOWN_FUNCTION(file);

PHP_FUNCTION(tempnam);
PHP_NAMED_FUNCTION(php_if_tmpfile);
PHP_NAMED_FUNCTION(php_if_fopen);
PHPAPI PHP_FUNCTION(fclose);
PHP_FUNCTION(popen);
PHP_FUNCTION(pclose);
PHPAPI PHP_FUNCTION(feof);
PHPAPI PHP_FUNCTION(fread);
PHPAPI PHP_FUNCTION(fgetc);
PHPAPI PHP_FUNCTION(fgets);
PHP_FUNCTION(fscanf);
PHPAPI PHP_FUNCTION(fgetss);
PHP_FUNCTION(fgetcsv);
PHPAPI PHP_FUNCTION(fwrite);
PHPAPI PHP_FUNCTION(fflush);
PHPAPI PHP_FUNCTION(rewind);
PHPAPI PHP_FUNCTION(ftell);
PHPAPI PHP_FUNCTION(fseek);
PHP_FUNCTION(mkdir);
PHP_FUNCTION(rmdir);
PHPAPI PHP_FUNCTION(fpassthru);
PHP_FUNCTION(readfile);
PHP_FUNCTION(umask);
PHP_FUNCTION(rename);
PHP_FUNCTION(unlink);
PHP_FUNCTION(copy);
PHP_FUNCTION(file);
PHP_FUNCTION(file_get_contents);
PHP_FUNCTION(file_write_content);
PHP_FUNCTION(set_socket_blocking); /* deprecated */
PHP_FUNCTION(stream_set_blocking);
PHP_FUNCTION(stream_select);
PHP_FUNCTION(stream_set_timeout);
PHP_FUNCTION(stream_set_write_buffer);
PHP_FUNCTION(stream_get_wrappers);
PHP_FUNCTION(stream_get_line);
PHP_FUNCTION(get_meta_tags);
PHP_FUNCTION(flock);
PHP_FUNCTION(fd_set);
PHP_FUNCTION(fd_isset);
#if (!defined(PHP_WIN32) && !defined(__BEOS__) && HAVE_REALPATH) || defined(ZTS)
PHP_FUNCTION(realpath);
PHP_FUNCTION(fnmatch);
#endif
PHP_NAMED_FUNCTION(php_if_ftruncate);
PHP_NAMED_FUNCTION(php_if_fstat);

PHP_FUNCTION(stream_get_meta_data);
PHP_FUNCTION(stream_register_wrapper);
PHP_FUNCTION(stream_context_create);
PHP_FUNCTION(stream_context_set_params);
PHP_FUNCTION(stream_context_set_option);
PHP_FUNCTION(stream_context_get_options);
PHP_FUNCTION(stream_filter_prepend);
PHP_FUNCTION(stream_filter_append);
PHP_MINIT_FUNCTION(user_streams);

PHPAPI int php_set_sock_blocking(int socketd, int block TSRMLS_DC);
PHPAPI int php_copy_file(char *src, char *dest TSRMLS_DC);

#define META_DEF_BUFSIZE 8192

#define PHP_FILE_USE_INCLUDE_PATH 1
#define PHP_FILE_IGNORE_NEW_LINES 2
#define PHP_FILE_SKIP_EMPTY_LINES 4

typedef enum _php_meta_tags_token {
	TOK_EOF = 0,
	TOK_OPENTAG,
	TOK_CLOSETAG,
	TOK_SLASH,
	TOK_EQUAL,
	TOK_SPACE,
	TOK_ID,
	TOK_STRING,
	TOK_OTHER
} php_meta_tags_token;

typedef struct _php_meta_tags_data {
	php_stream *stream;
	int ulc;
	int lc;
	char *input_buffer;
	char *token_data;
	int token_len;
	int in_meta;
} php_meta_tags_data;

php_meta_tags_token php_next_meta_token(php_meta_tags_data * TSRMLS_DC);

typedef struct {
  	int pclose_ret;
	size_t def_chunk_size;
	long auto_detect_line_endings;
	long default_socket_timeout;
	char *user_agent;
	char *user_stream_current_filename; /* for simple recursion protection */
} php_file_globals;

#ifdef ZTS
#define FG(v) TSRMG(file_globals_id, php_file_globals *, v)
extern int file_globals_id;
#else
#define FG(v) (file_globals.v)
extern php_file_globals file_globals;
#endif


#endif /* FILE_H */

