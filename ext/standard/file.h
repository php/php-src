/*
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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

#ifndef FILE_H
#define FILE_H

#include "php_network.h"

PHP_MINIT_FUNCTION(file);
PHP_MSHUTDOWN_FUNCTION(file);

PHPAPI PHP_FUNCTION(fclose);
PHPAPI PHP_FUNCTION(feof);
PHPAPI PHP_FUNCTION(fread);
PHPAPI PHP_FUNCTION(fgetc);
PHPAPI PHP_FUNCTION(fgets);
PHPAPI PHP_FUNCTION(fwrite);
PHPAPI PHP_FUNCTION(fflush);
PHPAPI PHP_FUNCTION(rewind);
PHPAPI PHP_FUNCTION(ftell);
PHPAPI PHP_FUNCTION(fseek);
PHPAPI PHP_FUNCTION(fpassthru);

PHP_MINIT_FUNCTION(user_streams);

PHPAPI int php_le_stream_context(void);
PHPAPI int php_set_sock_blocking(php_socket_t socketd, int block);
PHPAPI int php_copy_file(const char *src, const char *dest);
PHPAPI int php_copy_file_ex(const char *src, const char *dest, int src_chk);
PHPAPI int php_copy_file_ctx(const char *src, const char *dest, int src_chk, php_stream_context *ctx);
PHPAPI int php_mkdir_ex(const char *dir, zend_long mode, int options);
PHPAPI int php_mkdir(const char *dir, zend_long mode);
PHPAPI void php_fstat(php_stream *stream, zval *return_value);
PHPAPI void php_flock_common(php_stream *stream, zend_long operation, uint32_t operation_arg_num,
	zval *wouldblock, zval *return_value);

#define PHP_CSV_NO_ESCAPE EOF
PHPAPI void php_fgetcsv(php_stream *stream, char delimiter, char enclosure, int escape_char, size_t buf_len, char *buf, zval *return_value);
PHPAPI ssize_t php_fputcsv(php_stream *stream, zval *fields, char delimiter, char enclosure, int escape_char);

#define META_DEF_BUFSIZE 8192

#define PHP_FILE_USE_INCLUDE_PATH 1
#define PHP_FILE_IGNORE_NEW_LINES 2
#define PHP_FILE_SKIP_EMPTY_LINES 4
#define PHP_FILE_APPEND 8
#define PHP_FILE_NO_DEFAULT_CONTEXT 16

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

php_meta_tags_token php_next_meta_token(php_meta_tags_data *);

typedef struct {
	int pclose_ret;
	size_t def_chunk_size;
	zend_bool auto_detect_line_endings;
	zend_long default_socket_timeout;
	char *user_agent; /* for the http wrapper */
	char *from_address; /* for the ftp and http wrappers */
	const char *user_stream_current_filename; /* for simple recursion protection */
	php_stream_context *default_context;
	HashTable *stream_wrappers;			/* per-request copy of url_stream_wrappers_hash */
	HashTable *stream_filters;			/* per-request copy of stream_filters_hash */
	HashTable *wrapper_errors;			/* key: wrapper address; value: linked list of char* */
	int pclose_wait;
#if defined(HAVE_GETHOSTBYNAME_R)
	struct hostent tmp_host_info;
	char *tmp_host_buf;
	size_t tmp_host_buf_len;
#endif
} php_file_globals;

#ifdef ZTS
#define FG(v) ZEND_TSRMG(file_globals_id, php_file_globals *, v)
extern PHPAPI int file_globals_id;
#else
#define FG(v) (file_globals.v)
extern PHPAPI php_file_globals file_globals;
#endif


#endif /* FILE_H */
