/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_tokenizer.h"

typedef struct yy_buffer_state *YY_BUFFER_STATE;
typedef unsigned int yy_size_t;
struct yy_buffer_state
	{
	FILE *yy_input_file;
 
	char *yy_ch_buf;   /* input buffer */
	char *yy_buf_pos;  /* current position in input buffer */
 
	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;
 
	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;
 
	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;
 
	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;
 
	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;
 
	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;
 
	int yy_buffer_status;
#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2
	};

#include "zend.h"
#include "zend_language_scanner.h"
#include <zend_language_parser.h>

#define zendtext LANG_SCNG(yy_text)
#define zendleng LANG_SCNG(yy_leng)

/* {{{ tokenizer_functions[]
 *
 * Every user visible function must have an entry in tokenizer_functions[].
 */
zend_function_entry tokenizer_functions[] = {
	PHP_FE(token_get_all,	NULL)
	PHP_FE(token_name,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in tokenizer_functions[] */
};
/* }}} */

/* {{{ tokenizer_module_entry
 */
zend_module_entry tokenizer_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"tokenizer",
	tokenizer_functions,
	PHP_MINIT(tokenizer),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(tokenizer),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_TOKENIZER
ZEND_GET_MODULE(tokenizer)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(tokenizer)
{
	tokenizer_register_constants(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(tokenizer)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Tokenizer Support", "enabled");
	php_info_print_table_end();
}
/* }}} */

static void tokenize(zval *return_value TSRMLS_DC)
{
	zval token;
	zval *keyword;
	int token_type;
	zend_bool destroy;
	int token_line = 1;

	array_init(return_value);

	ZVAL_NULL(&token);
	while ((token_type = lex_scan(&token TSRMLS_CC))) {
		destroy = 1;
		switch (token_type) {
			case T_CLOSE_TAG:
				if (zendtext[zendleng - 1] != '>') {
					CG(zend_lineno)++;
				}
			case T_OPEN_TAG:
			case T_OPEN_TAG_WITH_ECHO:
			case T_WHITESPACE:
			case T_COMMENT:
			case T_DOC_COMMENT:
				destroy = 0;
				break;
		}

		if (token_type >= 256) {
			MAKE_STD_ZVAL(keyword);
			array_init(keyword);
			add_next_index_long(keyword, token_type);
			if (token_type == T_END_HEREDOC) {
				if (CG(increment_lineno)) {
					token_line = ++CG(zend_lineno);
					CG(increment_lineno) = 0;
				}
				add_next_index_stringl(keyword, Z_STRVAL(token), Z_STRLEN(token), 1);
				efree(Z_STRVAL(token));
			} else {
				add_next_index_stringl(keyword, zendtext, zendleng, 1);
			}
			add_next_index_long(keyword, token_line);
			add_next_index_zval(return_value, keyword);
		} else {
			add_next_index_stringl(return_value, zendtext, zendleng, 1);
		}
		if (destroy && Z_TYPE(token) != IS_NULL) {
			zval_dtor(&token);
		}
		ZVAL_NULL(&token);

		token_line = CG(zend_lineno);
	}
}

/* {{{ proto array token_get_all(string source)
 */
PHP_FUNCTION(token_get_all)
{
	char *source = NULL;
	int argc = ZEND_NUM_ARGS();
	int source_len;
	zval source_z;
	zend_lex_state original_lex_state;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &source, &source_len) == FAILURE) 
		return;

	ZVAL_STRINGL(&source_z, source, source_len, 1);
	zend_save_lexical_state(&original_lex_state TSRMLS_CC);

	if (zend_prepare_string_for_scanning(&source_z, "" TSRMLS_CC) == FAILURE) {
		RETURN_EMPTY_STRING();
	}

	LANG_SCNG(start) = 1;

	tokenize(return_value TSRMLS_CC);
	
	zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
	zval_dtor(&source_z);
}
/* }}} */

/* {{{ proto string token_name(int type)
 */
PHP_FUNCTION(token_name)
{
	int argc = ZEND_NUM_ARGS();
	long type;

	if (zend_parse_parameters(argc TSRMLS_CC, "l", &type) == FAILURE) {
		return;
	}
	RETVAL_STRING(get_token_type_name(type), 1);
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
