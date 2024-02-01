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
#include "zend_interfaces.h"

#include "tokenizer_data_arginfo.h"
#include "tokenizer_arginfo.h"

#define zendtext   LANG_SCNG(yy_text)
#define zendleng   LANG_SCNG(yy_leng)
#define zendcursor LANG_SCNG(yy_cursor)
#define zendlimit  LANG_SCNG(yy_limit)

zend_class_entry *php_token_ce;

/* {{{ tokenizer_module_entry */
zend_module_entry tokenizer_module_entry = {
	STANDARD_MODULE_HEADER,
	"tokenizer",
	ext_functions,
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

static zval *php_token_get_id(zval *obj) {
	zval *id = OBJ_PROP_NUM(Z_OBJ_P(obj), 0);
	if (Z_ISUNDEF_P(id)) {
		zend_throw_error(NULL,
			"Typed property PhpToken::$id must not be accessed before initialization");
		return NULL;
	}

	ZVAL_DEREF(id);
	ZEND_ASSERT(Z_TYPE_P(id) == IS_LONG);
	return id;
}

static zend_string *php_token_get_text(zval *obj) {
	zval *text_zval = OBJ_PROP_NUM(Z_OBJ_P(obj), 1);
	if (Z_ISUNDEF_P(text_zval)) {
		zend_throw_error(NULL,
			"Typed property PhpToken::$text must not be accessed before initialization");
		return NULL;
	}

	ZVAL_DEREF(text_zval);
	ZEND_ASSERT(Z_TYPE_P(text_zval) == IS_STRING);
	return Z_STR_P(text_zval);
}

static bool tokenize_common(
		zval *return_value, zend_string *source, zend_long flags, zend_class_entry *token_class);

PHP_METHOD(PhpToken, tokenize)
{
	zend_string *source;
	zend_long flags = 0;
	zend_class_entry *token_class;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(source)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	token_class = zend_get_called_scope(execute_data);

	/* Check construction preconditions in advance, so these are not repeated for each token. */
	if (token_class->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS) {
		zend_throw_error(NULL, "Cannot instantiate abstract class %s", ZSTR_VAL(token_class->name));
		RETURN_THROWS();
	}
	if (zend_update_class_constants(token_class) == FAILURE) {
		RETURN_THROWS();
	}

	if (!tokenize_common(return_value, source, flags, token_class)) {
		RETURN_THROWS();
	}
}

PHP_METHOD(PhpToken, __construct)
{
	zend_long id;
	zend_string *text;
	zend_long line = -1;
	zend_long pos = -1;
	zend_object *obj = Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_LONG(id)
		Z_PARAM_STR(text)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(line)
		Z_PARAM_LONG(pos)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_LONG(OBJ_PROP_NUM(obj, 0), id);
	zval_ptr_dtor(OBJ_PROP_NUM(obj, 1));
	ZVAL_STR_COPY(OBJ_PROP_NUM(obj, 1), text);
	ZVAL_LONG(OBJ_PROP_NUM(obj, 2), line);
	ZVAL_LONG(OBJ_PROP_NUM(obj, 3), pos);
}

PHP_METHOD(PhpToken, is)
{
	zval *kind;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(kind)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(kind) == IS_LONG) {
		zval *id_zval = php_token_get_id(ZEND_THIS);
		if (!id_zval) {
			RETURN_THROWS();
		}

		RETURN_BOOL(Z_LVAL_P(id_zval) == Z_LVAL_P(kind));
	} else if (Z_TYPE_P(kind) == IS_STRING) {
		zend_string *text = php_token_get_text(ZEND_THIS);
		if (!text) {
			RETURN_THROWS();
		}

		RETURN_BOOL(zend_string_equals(text, Z_STR_P(kind)));
	} else if (Z_TYPE_P(kind) == IS_ARRAY) {
		zval *id_zval = NULL, *entry;
		zend_string *text = NULL;
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(kind), entry) {
			ZVAL_DEREF(entry);
			if (Z_TYPE_P(entry) == IS_LONG) {
				if (!id_zval) {
					id_zval = php_token_get_id(ZEND_THIS);
					if (!id_zval) {
						RETURN_THROWS();
					}
				}
				if (Z_LVAL_P(id_zval) == Z_LVAL_P(entry)) {
					RETURN_TRUE;
				}
			} else if (Z_TYPE_P(entry) == IS_STRING) {
				if (!text) {
					text = php_token_get_text(ZEND_THIS);
					if (!text) {
						RETURN_THROWS();
					}
				}
				if (zend_string_equals(text, Z_STR_P(entry))) {
					RETURN_TRUE;
				}
			} else {
				zend_argument_type_error(1, "must only have elements of type string|int, %s given", zend_zval_type_name(entry));
				RETURN_THROWS();
			}
		} ZEND_HASH_FOREACH_END();
		RETURN_FALSE;
	} else {
		zend_argument_type_error(1, "must be of type string|int|array, %s given", zend_zval_type_name(kind));
		RETURN_THROWS();
	}
}

PHP_METHOD(PhpToken, isIgnorable)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval *id_zval = php_token_get_id(ZEND_THIS);
	if (!id_zval) {
		RETURN_THROWS();
	}

	zend_long id = Z_LVAL_P(id_zval);
	RETURN_BOOL(id == T_WHITESPACE || id == T_COMMENT || id == T_DOC_COMMENT || id == T_OPEN_TAG);
}

PHP_METHOD(PhpToken, getTokenName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval *id_zval = php_token_get_id(ZEND_THIS);
	if (!id_zval) {
		RETURN_THROWS();
	}

	if (Z_LVAL_P(id_zval) < 256) {
		RETURN_CHAR(Z_LVAL_P(id_zval));
	} else {
		const char *token_name = get_token_type_name(Z_LVAL_P(id_zval));
		if (!token_name) {
			RETURN_NULL();
		}

		RETURN_STRING(token_name);
	}
}

PHP_METHOD(PhpToken, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_string *text = php_token_get_text(ZEND_THIS);
	if (!text) {
		RETURN_THROWS();
	}

	RETURN_STR_COPY(text);
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(tokenizer)
{
	register_tokenizer_data_symbols(module_number);
	register_tokenizer_symbols(module_number);
	php_token_ce = register_class_PhpToken(zend_ce_stringable);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(tokenizer)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Tokenizer Support", "enabled");
	php_info_print_table_end();
}
/* }}} */

static zend_string *make_str(unsigned char *text, size_t leng, HashTable *interned_strings) {
	if (leng == 1) {
		return ZSTR_CHAR(text[0]);
	} else if (interned_strings) {
		zend_string *interned_str = zend_hash_str_find_ptr(interned_strings, (char *) text, leng);
		if (interned_str) {
			return zend_string_copy(interned_str);
		}
		interned_str = zend_string_init((char *) text, leng, 0);
		zend_hash_add_new_ptr(interned_strings, interned_str, interned_str);
		return interned_str;
	} else {
		return zend_string_init((char *) text, leng, 0);
	}
}

static void add_token(
		zval *return_value, int token_type, unsigned char *text, size_t leng, int lineno,
		zend_class_entry *token_class, HashTable *interned_strings) {
	zval token;
	if (token_class) {
		zend_object *obj = zend_objects_new(token_class);
		ZVAL_OBJ(&token, obj);
		ZVAL_LONG(OBJ_PROP_NUM(obj, 0), token_type);
		ZVAL_STR(OBJ_PROP_NUM(obj, 1), make_str(text, leng, interned_strings));
		ZVAL_LONG(OBJ_PROP_NUM(obj, 2), lineno);
		ZVAL_LONG(OBJ_PROP_NUM(obj, 3), text - LANG_SCNG(yy_start));

		/* If the class is extended with additional properties, initialized them as well. */
		if (UNEXPECTED(token_class->default_properties_count > 4)) {
			zval *dst = OBJ_PROP_NUM(obj, 4);
			zval *src = &token_class->default_properties_table[4];
			zval *end = token_class->default_properties_table
				+ token_class->default_properties_count;
			for (; src < end; src++, dst++) {
				ZVAL_COPY_PROP(dst, src);
			}
		}
	} else if (token_type >= 256) {
		array_init_size(&token, 3);
		zend_hash_real_init_packed(Z_ARRVAL(token));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL(token)) {
			ZEND_HASH_FILL_SET_LONG(token_type);
			ZEND_HASH_FILL_NEXT();
			ZEND_HASH_FILL_SET_STR(make_str(text, leng, interned_strings));
			ZEND_HASH_FILL_NEXT();
			ZEND_HASH_FILL_SET_LONG(lineno);
			ZEND_HASH_FILL_NEXT();
		} ZEND_HASH_FILL_END();
	} else {
		ZVAL_STR(&token, make_str(text, leng, interned_strings));
	}
	zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &token);
}

static bool tokenize(zval *return_value, zend_string *source, zend_class_entry *token_class)
{
	zval source_zval;
	zend_lex_state original_lex_state;
	zval token;
	int token_type;
	int token_line = 1;
	int need_tokens = -1; /* for __halt_compiler lexing. -1 = disabled */
	HashTable interned_strings;

	ZVAL_STR_COPY(&source_zval, source);
	zend_save_lexical_state(&original_lex_state);

	zend_prepare_string_for_scanning(&source_zval, ZSTR_EMPTY_ALLOC());

	LANG_SCNG(yy_state) = yycINITIAL;
	zend_hash_init(&interned_strings, 0, NULL, NULL, 0);
	array_init(return_value);

	while ((token_type = lex_scan(&token, NULL))) {
		ZEND_ASSERT(token_type != T_ERROR);

		add_token(
			return_value, token_type, zendtext, zendleng, token_line,
			token_class, &interned_strings);

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
				if (zendcursor < zendlimit) {
					add_token(
						return_value, T_INLINE_HTML, zendcursor, zendlimit - zendcursor,
						token_line, token_class, &interned_strings);
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
	zend_hash_destroy(&interned_strings);

	return 1;
}

struct event_context {
	zval *tokens;
	zend_class_entry *token_class;
};

static zval *extract_token_id_to_replace(zval *token_zv, const char *text, size_t length) {
	zval *id_zv, *text_zv;
	ZEND_ASSERT(token_zv);
	if (Z_TYPE_P(token_zv) == IS_ARRAY) {
		id_zv = zend_hash_index_find(Z_ARRVAL_P(token_zv), 0);
		text_zv = zend_hash_index_find(Z_ARRVAL_P(token_zv), 1);
	} else if (Z_TYPE_P(token_zv) == IS_OBJECT) {
		id_zv = OBJ_PROP_NUM(Z_OBJ_P(token_zv), 0);
		text_zv = OBJ_PROP_NUM(Z_OBJ_P(token_zv), 1);
	} else {
		return NULL;
	}

	/* There are multiple candidate tokens to which this feedback may apply,
	 * check text to make sure this is the right one. */
	ZEND_ASSERT(Z_TYPE_P(text_zv) == IS_STRING);
	if (Z_STRLEN_P(text_zv) == length && !memcmp(Z_STRVAL_P(text_zv), text, length)) {
		return id_zv;
	}
	return NULL;
}

void on_event(
		zend_php_scanner_event event, int token, int line,
		const char *text, size_t length, void *context)
{
	struct event_context *ctx = context;

	switch (event) {
		case ON_TOKEN:
			if (token == END) break;
			/* Special cases */
			if (token == ';' && LANG_SCNG(yy_leng) > 1) { /* ?> or ?>\n or ?>\r\n */
				token = T_CLOSE_TAG;
			} else if (token == T_ECHO && LANG_SCNG(yy_leng) == sizeof("<?=") - 1) {
				token = T_OPEN_TAG_WITH_ECHO;
			}
			add_token(
				ctx->tokens, token, (unsigned char *) text, length, line, ctx->token_class, NULL);
			break;
		case ON_FEEDBACK: {
			HashTable *tokens_ht = Z_ARRVAL_P(ctx->tokens);
			zval *token_zv, *id_zv = NULL;
			ZEND_HASH_REVERSE_FOREACH_VAL(tokens_ht, token_zv) {
				id_zv = extract_token_id_to_replace(token_zv, text, length);
				if (id_zv) {
					break;
				}
			} ZEND_HASH_FOREACH_END();
			ZEND_ASSERT(id_zv);
			ZVAL_LONG(id_zv, token);
			break;
		}
		case ON_STOP:
			if (LANG_SCNG(yy_cursor) != LANG_SCNG(yy_limit)) {
				add_token(ctx->tokens, T_INLINE_HTML, LANG_SCNG(yy_cursor),
					LANG_SCNG(yy_limit) - LANG_SCNG(yy_cursor), CG(zend_lineno),
					ctx->token_class, NULL);
			}
			break;
	}
}

static bool tokenize_parse(
		zval *return_value, zend_string *source, zend_class_entry *token_class)
{
	zval source_zval;
	struct event_context ctx;
	zval token_stream;
	zend_lex_state original_lex_state;
	bool original_in_compilation;
	bool success;

	ZVAL_STR_COPY(&source_zval, source);

	original_in_compilation = CG(in_compilation);
	CG(in_compilation) = 1;
	zend_save_lexical_state(&original_lex_state);

	zend_prepare_string_for_scanning(&source_zval, ZSTR_EMPTY_ALLOC());
	array_init(&token_stream);

	ctx.tokens = &token_stream;
	ctx.token_class = token_class;

	CG(ast) = NULL;
	CG(ast_arena) = zend_arena_create(1024 * 32);
	LANG_SCNG(yy_state) = yycINITIAL;
	LANG_SCNG(on_event) = on_event;
	LANG_SCNG(on_event_context) = &ctx;

	if((success = (zendparse() == SUCCESS))) {
		ZVAL_COPY_VALUE(return_value, &token_stream);
	} else {
		zval_ptr_dtor(&token_stream);
	}

	zend_ast_destroy(CG(ast));
	zend_arena_destroy(CG(ast_arena));

	/* restore compiler and scanner global states */
	zend_restore_lexical_state(&original_lex_state);
	CG(in_compilation) = original_in_compilation;

	zval_ptr_dtor_str(&source_zval);

	return success;
}

static bool tokenize_common(
		zval *return_value, zend_string *source, zend_long flags, zend_class_entry *token_class)
{
	if (flags & TOKEN_PARSE) {
		return tokenize_parse(return_value, source, token_class);
	} else {
		int success = tokenize(return_value, source, token_class);
		/* Normal token_get_all() should not throw. */
		zend_clear_exception();
		return success;
	}
}

/* }}} */

/* {{{ */
PHP_FUNCTION(token_get_all)
{
	zend_string *source;
	zend_long flags = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(source)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	if (!tokenize_common(return_value, source, flags, /* token_class */ NULL)) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ */
PHP_FUNCTION(token_name)
{
	zend_long type;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(type)
	ZEND_PARSE_PARAMETERS_END();

	const char *token_name = get_token_type_name(type);
	if (!token_name) {
		token_name = "UNKNOWN";
	}
	RETURN_STRING(token_name);
}
/* }}} */
