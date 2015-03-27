/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

typedef struct {
	zval *tokens;
	int	index,
		length,
		current,
		in_class,
		in_trait_use,
		in_const_list;
} t_stream;

/* This function will be soon generated based on zend_language_parser.y
 */
static zend_bool t_is_semi_reserved(int token_type)
{
	switch (token_type) {
	/* [[[ semi-reserved */
		case T_INCLUDE:
		case T_INCLUDE_ONCE:
		case T_EVAL:
		case T_REQUIRE:
		case T_REQUIRE_ONCE:
		case T_LOGICAL_OR:
		case T_LOGICAL_XOR:
		case T_LOGICAL_AND:
		case T_INSTANCEOF:
		case T_NEW:
		case T_CLONE:
		case T_EXIT:
		case T_IF:
		case T_ELSEIF:
		case T_ELSE:
		case T_ENDIF:
		case T_ECHO:
		case T_DO:
		case T_WHILE:
		case T_ENDWHILE:
		case T_FOR:
		case T_ENDFOR:
		case T_FOREACH:
		case T_ENDFOREACH:
		case T_DECLARE:
		case T_ENDDECLARE:
		case T_AS:
		case T_TRY:
		case T_CATCH:
		case T_FINALLY:
		case T_THROW:
		case T_USE:
		case T_INSTEADOF:
		case T_GLOBAL:
		case T_VAR:
		case T_UNSET:
		case T_ISSET:
		case T_EMPTY:
		case T_CONTINUE:
		case T_GOTO:
		case T_FUNCTION:
		case T_CONST:
		case T_RETURN:
		case T_PRINT:
		case T_YIELD:
		case T_LIST:
		case T_SWITCH:
		case T_ENDSWITCH:
		case T_CASE:
		case T_DEFAULT:
		case T_BREAK:
		case T_ARRAY:
		case T_CALLABLE:
		case T_EXTENDS:
		case T_IMPLEMENTS:
		case T_NAMESPACE:
		case T_TRAIT:
		case T_INTERFACE:
	/* ]]] */
		return 1;
	}
	return 0;
}

static zend_always_inline t_stream *t_init(zval *tokens)
{
	t_stream *ts = (t_stream *)malloc (sizeof (t_stream));
	ts->index = ts->current = -1;
	ts->in_class = ts->in_trait_use = ts->in_const_list = 0;
	ts->tokens = tokens;
	ts->length = zend_hash_num_elements(Z_ARRVAL_P(tokens));
	return ts;
}

static int t_get_type(t_stream *ts, int index)
{
	zval *token = zend_hash_index_find(Z_ARRVAL_P(ts->tokens), index);
	switch (Z_TYPE_P(token)) {
		case IS_ARRAY:
			return zval_get_long(zend_hash_index_find(Z_ARRVAL_P(token), 0));
		case IS_STRING:
			return Z_STRVAL_P(token)[0];
	}
	ZEND_ASSERT(0);
}

static zend_always_inline zend_bool t_next(t_stream *ts)
{
	return (++ts->index < ts->length) ? ts->current = t_get_type(ts, ts->index) : 0;
}

static zend_always_inline zend_bool t_previous(t_stream *ts)
{
	return (--ts->index >= 0) ? ts->current = t_get_type(ts, ts->index) : 0;
}

static int t_look(t_stream *ts, zend_bool ahead, int delta)
{
	int	t_type,
		index = ts->index;

	while (delta && ((ahead ? ++index : --index) < ts->length)) {
		switch (t_type = t_get_type(ts, index)) {
			case T_WHITESPACE: case T_COMMENT: case T_DOC_COMMENT:
				break;
			case T_STRING:
				return index;
			default:
				if (t_is_semi_reserved(t_type)) return index;
				else delta--;
		}
	}
	return -1;
}

static zend_always_inline void t_stringify(t_stream *ts, int index)
{
	zval *token = zend_hash_index_find(Z_ARRVAL_P(ts->tokens), index);

	if (Z_TYPE_P(token) == IS_ARRAY)
		ZVAL_LONG(zend_hash_index_find(Z_ARRVAL_P(token), 0), T_STRING);
}

#define t_stringify_current(ts) t_stringify(ts, ts->index)

static zend_always_inline void t_stringify_next(t_stream *ts, int delta)
{
	if (-1 != (delta = t_look(ts, 1, delta))) t_stringify(ts, delta);
}

static zend_always_inline void t_stringify_previous(t_stream *ts, int delta)
{
	if (-1 != (delta = t_look(ts, 0, delta))) t_stringify(ts, delta);
}

static void t_parse(zval *tokens)
{
	t_stream *ts = t_init(tokens);

	while (t_next(ts)) {
		switch (ts->current) {
			case T_WHITESPACE: case T_COMMENT: case T_DOC_COMMENT: case T_STRING: case T_INLINE_HTML:
				continue;
			case T_CLASS: case T_TRAIT: case T_INTERFACE:
				ts->in_class++;
				continue;
			case T_PAAMAYIM_NEKUDOTAYIM: case T_OBJECT_OPERATOR:
				t_stringify_next(ts, 1);
				continue;
		}

		if (ts->in_class) {
			switch (ts->current) {
				case T_CURLY_OPEN: case T_DOLLAR_OPEN_CURLY_BRACES:
					ts->in_class++;
					break;
				case '{':
					ts->in_class++;
					if (ts->in_trait_use) while (ts->in_trait_use && t_next(ts)) {
						switch (ts->current) { /* use ... { ... } */
							case T_PAAMAYIM_NEKUDOTAYIM: case T_OBJECT_OPERATOR:
								t_stringify_next(ts, 1);
								break;
							case T_AS:
								/* T_STRING<?> T_AS visibility? T_STRING<?>; */
								if (-1 != t_look(ts, 0, 1)) {
									t_stringify_previous(ts, 1);
									t_stringify_next(ts, 2);
								}
								break;
							case '}':
								ts->in_trait_use = 0;
								t_previous(ts);
								break;
						}
					}
					break;
				case '}':
					if (1 == --ts->in_class) ts->in_class--;
					break;
				case T_FUNCTION:
					if(2 == ts->in_class) t_stringify_next(ts, 1);
					break;
				case T_USE:
					ts->in_trait_use++;
					break;
				case T_CONST:
					ts->in_const_list++;
					while (ts->in_const_list && t_next(ts)) { /* const ...; */
						switch (ts->current) {
							case T_PAAMAYIM_NEKUDOTAYIM: case T_OBJECT_OPERATOR:
								t_stringify_next(ts, 1);
								break;
							case '(': case '[':
								ts->in_const_list++;
								break;
							case ')': case ']':
								ts->in_const_list--;
								break;
							case '=':
								if (1 == ts->in_const_list) t_stringify_previous(ts, 1);
								break;
							case ';':
								if (1 == ts->in_const_list) ts->in_const_list--;
								break;
						}
					}
					break;
				case '(':
				case ';':
					if (ts->in_trait_use) ts->in_trait_use = 0;
					break;
			}
		}
	}
	free(ts);
}

static void tokenize(zval *return_value)
{
	zval token;
	zval keyword;
	int token_type;
	zend_bool destroy;
	int token_line = 1;
	int need_tokens = -1; /* for __halt_compiler lexing. -1 = disabled */

	array_init(return_value);

	ZVAL_NULL(&token);
	while ((token_type = lex_scan(&token))) {
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
			array_init(&keyword);
			add_next_index_long(&keyword, token_type);
			if (token_type == T_END_HEREDOC) {
				if (CG(increment_lineno)) {
					token_line = ++CG(zend_lineno);
					CG(increment_lineno) = 0;
				}
			}
			add_next_index_stringl(&keyword, (char *)zendtext, zendleng);
			add_next_index_long(&keyword, token_line);
			add_next_index_zval(return_value, &keyword);
		} else {
			add_next_index_stringl(return_value, (char *)zendtext, zendleng);
		}
		if (destroy && Z_TYPE(token) != IS_NULL) {
			zval_dtor(&token);
		}
		ZVAL_NULL(&token);

		/* after T_HALT_COMPILER collect the next three non-dropped tokens */
		if (need_tokens != -1) {
			if (token_type != T_WHITESPACE && token_type != T_OPEN_TAG
				&& token_type != T_COMMENT && token_type != T_DOC_COMMENT
				&& --need_tokens == 0
			) {
				/* fetch the rest into a T_INLINE_HTML */
				if (zendcursor != zendlimit) {
					array_init(&keyword);
					add_next_index_long(&keyword, T_INLINE_HTML);
					add_next_index_stringl(&keyword, (char *)zendcursor, zendlimit - zendcursor);
					add_next_index_long(&keyword, token_line);
					add_next_index_zval(return_value, &keyword);
				}
				break;
			}
		} else if (token_type == T_HALT_COMPILER) {
			need_tokens = 3;
		}

		token_line = CG(zend_lineno);
	}

	t_parse(return_value);
}

/* {{{ proto array token_get_all(string source)
 */
PHP_FUNCTION(token_get_all)
{
	zend_string *source;
	zval source_zval;
	zend_lex_state original_lex_state;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &source) == FAILURE) {
		return;
	}

	ZVAL_STR_COPY(&source_zval, source);
	zend_save_lexical_state(&original_lex_state);

	if (zend_prepare_string_for_scanning(&source_zval, "") == FAILURE) {
		zend_restore_lexical_state(&original_lex_state);
		RETURN_FALSE;
	}

	LANG_SCNG(yy_state) = yycINITIAL;

	tokenize(return_value);

	zend_restore_lexical_state(&original_lex_state);
	zval_dtor(&source_zval);
}
/* }}} */

/* {{{ proto string token_name(int type)
 */
PHP_FUNCTION(token_name)
{
	zend_long type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &type) == FAILURE) {
		return;
	}

	RETVAL_STRING(get_token_type_name(type));
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
