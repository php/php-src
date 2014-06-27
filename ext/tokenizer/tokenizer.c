/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

#include "zend.h"
#include "zend_language_scanner.h"
#include "zend_language_scanner_defs.h"
#include <zend_language_parser.h>

#define zendtext   LANG_SCNG(yy_text)
#define zendleng   LANG_SCNG(yy_leng)
#define zendcursor LANG_SCNG(yy_cursor)
#define zendlimit  LANG_SCNG(yy_limit)

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_token_get_all, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_token_name, 0, 0, 1)
	ZEND_ARG_INFO(0, token)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ tokenizer_functions[]
 *
 * Every user visible function must have an entry in tokenizer_functions[].
 */
const zend_function_entry tokenizer_functions[] = {
	PHP_FE(token_get_all,	arginfo_token_get_all)
	PHP_FE(token_name,		arginfo_token_name)
	PHP_FE_END
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
	int need_tokens = -1; // for __halt_compiler lexing. -1 = disabled

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
			}
			add_next_index_stringl(keyword, (char *)zendtext, zendleng, 1);
			add_next_index_long(keyword, token_line);
			add_next_index_zval(return_value, keyword);
		} else {
			add_next_index_stringl(return_value, (char *)zendtext, zendleng, 1);
		}
		if (destroy && Z_TYPE(token) != IS_NULL) {
			zval_dtor(&token);
		}
		ZVAL_NULL(&token);

		// after T_HALT_COMPILER collect the next three non-dropped tokens
		if (need_tokens != -1) {
			if (token_type != T_WHITESPACE && token_type != T_OPEN_TAG
			    && token_type != T_COMMENT && token_type != T_DOC_COMMENT
			    && --need_tokens == 0
			) {
				// fetch the rest into a T_INLINE_HTML
				if (zendcursor != zendlimit) {
					MAKE_STD_ZVAL(keyword);
					array_init(keyword);
					add_next_index_long(keyword, T_INLINE_HTML);
					add_next_index_stringl(keyword, (char *)zendcursor, zendlimit - zendcursor, 1);
					add_next_index_long(keyword, token_line);
					add_next_index_zval(return_value, keyword);
				}
				break;
			}
		} else if (token_type == T_HALT_COMPILER) {
			need_tokens = 3;
		}

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

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}

	ZVAL_STRINGL(&source_z, source, source_len, 1);
	zend_save_lexical_state(&original_lex_state TSRMLS_CC);

	if (zend_prepare_string_for_scanning(&source_z, "" TSRMLS_CC) == FAILURE) {
		zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
		RETURN_FALSE;
	}

	LANG_SCNG(yy_state) = yycINITIAL;

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
