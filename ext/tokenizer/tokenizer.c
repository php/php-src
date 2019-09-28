/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_tokenizer.h"

#include "zend.h"
#include "zend_exceptions.h"
#include "zend_language_scanner.h"
#include "zend_language_scanner_defs.h"
#include <zend_language_parser.h>

#define zendtext   LANG_SCNG(yy_text)
#define zendleng   LANG_SCNG(yy_leng)
#define zendcursor LANG_SCNG(yy_cursor)
#define zendlimit  LANG_SCNG(yy_limit)

#define TOKEN_PARSE 				1

void tokenizer_token_get_all_register_constants(INIT_FUNC_ARGS) {
	REGISTER_LONG_CONSTANT("TOKEN_PARSE", TOKEN_PARSE, CONST_CS|CONST_PERSISTENT);
}

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_token_get_all, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_token_name, 0, 0, 1)
	ZEND_ARG_INFO(0, token)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ tokenizer_functions[]
 *
 * Every user visible function must have an entry in tokenizer_functions[].
 */
static const zend_function_entry tokenizer_functions[] = {
	PHP_FE(token_get_all,	arginfo_token_get_all)
	PHP_FE(token_name,		arginfo_token_name)
	PHP_FE_END
};
/* }}} */

/* {{{ tokenizer_module_entry
 */
zend_module_entry tokenizer_module_entry = {
	STANDARD_MODULE_HEADER,
	"tokenizer",
	tokenizer_functions,
	PHP_MINIT(tokenizer),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(tokenizer),
	PHP_TOKENIZER_VERSION,
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
	tokenizer_token_get_all_register_constants(INIT_FUNC_ARGS_PASSTHRU);
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

static void add_token(zval *return_value, int token_type,
		unsigned char *text, size_t leng, int lineno) {
	if (token_type >= 256) {
		zval keyword;
		array_init(&keyword);
		add_next_index_long(&keyword, token_type);
		if (leng == 1) {
			add_next_index_str(&keyword, ZSTR_CHAR(text[0]));
		} else {
			add_next_index_stringl(&keyword, (char *) text, leng);
		}
		add_next_index_long(&keyword, lineno);
		add_next_index_zval(return_value, &keyword);
	} else {
		if (leng == 1) {
			add_next_index_str(return_value, ZSTR_CHAR(text[0]));
		} else {
			add_next_index_stringl(return_value, (char *) text, leng);
		}
	}
}

static zend_bool tokenize(zval *return_value, zend_string *source)
{
	zval source_zval;
	zend_lex_state original_lex_state;
	zval token;
	int token_type;
	int token_line = 1;
	int need_tokens = -1; /* for __halt_compiler lexing. -1 = disabled */

	ZVAL_STR_COPY(&source_zval, source);
	zend_save_lexical_state(&original_lex_state);

	if (zend_prepare_string_for_scanning(&source_zval, "") == FAILURE) {
		zend_restore_lexical_state(&original_lex_state);
		return 0;
	}

	LANG_SCNG(yy_state) = yycINITIAL;
	array_init(return_value);

	while ((token_type = lex_scan(&token, NULL))) {
		add_token(return_value, token_type, zendtext, zendleng, token_line);

		if (Z_TYPE(token) != IS_UNDEF) {
			zval_ptr_dtor_nogc(&token);
			ZVAL_UNDEF(&token);
		}

		/* after T_HALT_COMPILER collect the next three non-dropped tokens */
		if (need_tokens != -1) {
			if (token_type != T_WHITESPACE && token_type != T_OPEN_TAG
				&& token_type != T_COMMENT && token_type != T_DOC_COMMENT
				&& --need_tokens == 0
			) {
				/* fetch the rest into a T_INLINE_HTML */
				if (zendcursor != zendlimit) {
					add_token(return_value, T_INLINE_HTML,
						zendcursor, zendlimit - zendcursor, token_line);
				}
				break;
			}
		} else if (token_type == T_HALT_COMPILER) {
			need_tokens = 3;
		}

		if (CG(increment_lineno)) {
			CG(zend_lineno)++;
			CG(increment_lineno) = 0;
		}

		token_line = CG(zend_lineno);
	}

	zval_ptr_dtor_str(&source_zval);
	zend_restore_lexical_state(&original_lex_state);

	return 1;
}

void on_event(zend_php_scanner_event event, int token, int line, void *context)
{
	zval *token_stream = (zval *) context;
	HashTable *tokens_ht;
	zval *token_zv;

	switch (event) {
		case ON_TOKEN:
			{
				if (token == END) break;
				/* Special cases */
				if (token == ';' && LANG_SCNG(yy_leng) > 1) { /* ?> or ?>\n or ?>\r\n */
					token = T_CLOSE_TAG;
				} else if (token == T_ECHO && LANG_SCNG(yy_leng) == sizeof("<?=") - 1) {
					token = T_OPEN_TAG_WITH_ECHO;
				}
				add_token(token_stream, token, LANG_SCNG(yy_text), LANG_SCNG(yy_leng), line);
			}
			break;
		case ON_FEEDBACK:
			tokens_ht = Z_ARRVAL_P(token_stream);
			token_zv = zend_hash_index_find(tokens_ht, zend_hash_num_elements(tokens_ht) - 1);
			if (token_zv && Z_TYPE_P(token_zv) == IS_ARRAY) {
				ZVAL_LONG(zend_hash_index_find(Z_ARRVAL_P(token_zv), 0), token);
			}
			break;
		case ON_STOP:
			if (LANG_SCNG(yy_cursor) != LANG_SCNG(yy_limit)) {
				add_token(token_stream, T_INLINE_HTML, LANG_SCNG(yy_cursor),
					LANG_SCNG(yy_limit) - LANG_SCNG(yy_cursor), CG(zend_lineno));
			}
			break;
	}
}

static zend_bool tokenize_parse(zval *return_value, zend_string *source)
{
	zval source_zval;
	zend_lex_state original_lex_state;
	zend_bool original_in_compilation;
	zend_bool success;

	ZVAL_STR_COPY(&source_zval, source);

	original_in_compilation = CG(in_compilation);
	CG(in_compilation) = 1;
	zend_save_lexical_state(&original_lex_state);

	if ((success = (zend_prepare_string_for_scanning(&source_zval, "") == SUCCESS))) {
		zval token_stream;
		array_init(&token_stream);

		CG(ast) = NULL;
		CG(ast_arena) = zend_arena_create(1024 * 32);
		LANG_SCNG(yy_state) = yycINITIAL;
		LANG_SCNG(on_event) = on_event;
		LANG_SCNG(on_event_context) = &token_stream;

		if((success = (zendparse() == SUCCESS))) {
			ZVAL_COPY_VALUE(return_value, &token_stream);
		} else {
			zval_ptr_dtor(&token_stream);
		}

		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
	}

	/* restore compiler and scanner global states */
	zend_restore_lexical_state(&original_lex_state);
	CG(in_compilation) = original_in_compilation;

	zval_ptr_dtor_str(&source_zval);

	return success;
}

/* }}} */

/* {{{ proto array token_get_all(string source [, int flags])
 */
PHP_FUNCTION(token_get_all)
{
	zend_string *source;
	zend_long flags = 0;
	zend_bool success;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(source)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	if (flags & TOKEN_PARSE) {
		success = tokenize_parse(return_value, source);
	} else {
		success = tokenize(return_value, source);
		/* Normal token_get_all() should not throw. */
		zend_clear_exception();
	}

	if (!success) RETURN_FALSE;
}
/* }}} */

/* {{{ proto string token_name(int type)
 */
PHP_FUNCTION(token_name)
{
	zend_long type;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(type)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_STRING(get_token_type_name(type));
}
/* }}} */
