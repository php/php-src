%require "3.0"
%code top {
/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_json.h"
#include "php_json_parser.h"

#define YYDEBUG 0

#if YYDEBUG
int json_yydebug = 1;
#endif

#ifdef _MSC_VER
#define YYMALLOC malloc
#define YYFREE free
#endif

#define PHP_JSON_DEPTH_DEC --parser->depth
#define PHP_JSON_DEPTH_INC \
	if (parser->max_depth && parser->depth >= parser->max_depth) { \
		parser->scanner.errcode = PHP_JSON_ERROR_DEPTH; \
		YYERROR; \
	} \
	++parser->depth

}

%define api.prefix {php_json_yy}
%define api.pure full
%param  { php_json_parser *parser  }

%union {
	zval value;
}

%token <value> PHP_JSON_T_NUL
%token <value> PHP_JSON_T_TRUE
%token <value> PHP_JSON_T_FALSE
%token <value> PHP_JSON_T_INT
%token <value> PHP_JSON_T_DOUBLE
%token <value> PHP_JSON_T_STRING
%token <value> PHP_JSON_T_ESTRING
%token PHP_JSON_T_EOI
%token PHP_JSON_T_ERROR

%type <value> start object key value array
%type <value> members member elements element

%destructor { zval_ptr_dtor_nogc(&$$); } <value>

%code {
static int php_json_yylex(union YYSTYPE *value, php_json_parser *parser);
static void php_json_yyerror(php_json_parser *parser, char const *msg);
static int php_json_parser_array_create(php_json_parser *parser, zval *array);
static int php_json_parser_object_create(php_json_parser *parser, zval *array);

}

%% /* Rules */

start:
		value PHP_JSON_T_EOI
			{
				ZVAL_COPY_VALUE(&$$, &$1);
				ZVAL_COPY_VALUE(parser->return_value, &$1);
				(void) php_json_yynerrs;
				YYACCEPT;
			}
;

object:
		'{'
			{
				PHP_JSON_DEPTH_INC;
				if (parser->methods.object_start && FAILURE == parser->methods.object_start(parser)) {
					YYERROR;
				}
			}
		members object_end
			{
				ZVAL_COPY_VALUE(&$$, &$3);
				PHP_JSON_DEPTH_DEC;
				if (parser->methods.object_end && FAILURE == parser->methods.object_end(parser, &$$)) {
					YYERROR;
				}
			}
;

object_end:
		'}'
	|	']'
			{
				parser->scanner.errcode = PHP_JSON_ERROR_STATE_MISMATCH;
				YYERROR;
			}
;

members:
		%empty
			{
				if ((parser->scanner.options & PHP_JSON_OBJECT_AS_ARRAY) && parser->methods.object_create == php_json_parser_object_create) {
					ZVAL_EMPTY_ARRAY(&$$);
				} else {
					parser->methods.object_create(parser, &$$);
				}
			}
	|	member
;

member:
		key ':' value
			{
				parser->methods.object_create(parser, &$$);
				if (parser->methods.object_update(parser, &$$, Z_STR($1), &$3) == FAILURE) {
					YYERROR;
				}
			}
	|	member ',' key ':' value
			{
				if (parser->methods.object_update(parser, &$1, Z_STR($3), &$5) == FAILURE) {
					YYERROR;
				}
				ZVAL_COPY_VALUE(&$$, &$1);
			}
;

array:
		'['
			{
				PHP_JSON_DEPTH_INC;
				if (parser->methods.array_start && FAILURE == parser->methods.array_start(parser)) {
					YYERROR;
				}
			}
		elements array_end
			{
				ZVAL_COPY_VALUE(&$$, &$3);
				PHP_JSON_DEPTH_DEC;
				if (parser->methods.array_end && FAILURE == parser->methods.array_end(parser, &$$)) {
					YYERROR;
				}
			}
;

array_end:
		']'
	|	'}'
			{
				parser->scanner.errcode = PHP_JSON_ERROR_STATE_MISMATCH;
				YYERROR;
			}
;

elements:
		%empty
			{
				if (parser->methods.array_create == php_json_parser_array_create) {
					ZVAL_EMPTY_ARRAY(&$$);
				} else {
					parser->methods.array_create(parser, &$$);
				}
			}
	|	element
;

element:
		value
			{
				parser->methods.array_create(parser, &$$);
				parser->methods.array_append(parser, &$$, &$1);
			}
	|	element ',' value
			{
				parser->methods.array_append(parser, &$1, &$3);
				ZVAL_COPY_VALUE(&$$, &$1);
			}
;

key:
		PHP_JSON_T_STRING
	|	PHP_JSON_T_ESTRING
;

value:
		object
	|	array
	|	PHP_JSON_T_STRING
	|	PHP_JSON_T_ESTRING
	|	PHP_JSON_T_INT
	|	PHP_JSON_T_DOUBLE
	|	PHP_JSON_T_NUL
	|	PHP_JSON_T_TRUE
	|	PHP_JSON_T_FALSE
;

%% /* Functions */

static int php_json_parser_array_create(php_json_parser *parser, zval *array)
{
	array_init(array);
	return SUCCESS;
}

static int php_json_parser_array_append(php_json_parser *parser, zval *array, zval *zvalue)
{
	zend_hash_next_index_insert(Z_ARRVAL_P(array), zvalue);
	return SUCCESS;
}

static int php_json_parser_object_create(php_json_parser *parser, zval *object)
{
	if (parser->scanner.options & PHP_JSON_OBJECT_AS_ARRAY) {
		array_init(object);
	} else {
		object_init(object);
	}
	return SUCCESS;
}

static int php_json_parser_object_update(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	/* if JSON_OBJECT_AS_ARRAY is set */
	if (Z_TYPE_P(object) == IS_ARRAY) {
		zend_symtable_update(Z_ARRVAL_P(object), key, zvalue);
	} else {
		if (ZSTR_LEN(key) > 0 && ZSTR_VAL(key)[0] == '\0') {
			parser->scanner.errcode = PHP_JSON_ERROR_INVALID_PROPERTY_NAME;
			zend_string_release_ex(key, 0);
			zval_ptr_dtor_nogc(zvalue);
			zval_ptr_dtor_nogc(object);
			return FAILURE;
		}
		zend_std_write_property(Z_OBJ_P(object), key, zvalue, NULL);
		Z_TRY_DELREF_P(zvalue);
	}
	zend_string_release_ex(key, 0);

	return SUCCESS;
}

static int php_json_parser_array_create_validate(php_json_parser *parser, zval *array)
{
	ZVAL_NULL(array);
	return SUCCESS;
}

static int php_json_parser_array_append_validate(php_json_parser *parser, zval *array, zval *zvalue)
{
	return SUCCESS;
}

static int php_json_parser_object_create_validate(php_json_parser *parser, zval *object)
{
	ZVAL_NULL(object);
	return SUCCESS;
}

static int php_json_parser_object_update_validate(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	return SUCCESS;
}

static int php_json_yylex(union YYSTYPE *value, php_json_parser *parser)
{
	int token = php_json_scan(&parser->scanner);

	bool validate = parser->methods.array_create == php_json_parser_array_create_validate
		&& parser->methods.array_append == php_json_parser_array_append_validate
		&& parser->methods.object_create == php_json_parser_object_create_validate
		&& parser->methods.object_update == php_json_parser_object_update_validate;

	if (validate) {
		zval_ptr_dtor_str(&(parser->scanner.value));
		ZVAL_UNDEF(&value->value);
	} else {
		value->value = parser->scanner.value;
	}

	return token;
}

static void php_json_yyerror(php_json_parser *parser, char const *msg)
{
	if (!parser->scanner.errcode) {
		parser->scanner.errcode = PHP_JSON_ERROR_SYNTAX;
	}
}

PHP_JSON_API php_json_error_code php_json_parser_error_code(const php_json_parser *parser)
{
	return parser->scanner.errcode;
}

PHP_JSON_API size_t php_json_parser_error_line(const php_json_parser *parser)
{
	return parser->scanner.line;
}

static zend_always_inline bool php_json_is_hex(php_json_ctype c, php_json_ctype lo, php_json_ctype hi)
{
	php_json_ctype l = c | 0x20; /* fold ASCII case */
	return l >= (lo | 0x20) && l <= (hi | 0x20);
}

PHP_JSON_API size_t php_json_parser_error_column(const php_json_parser *parser)
{
	const php_json_scanner *s = &parser->scanner;
	const php_json_ctype *p = s->line_start;
	const php_json_ctype *end = s->token;
	/* Column is computed lazily here (only on error), by replaying the scanner's
	 * per-token column rules over the bytes between the start of the current line
	 * and the failing token. This keeps the decode success path free of any
	 * per-character column bookkeeping while preserving the reported columns:
	 * each \uXXXX escape and each \uXXXX\uXXXX surrogate pair counts as one
	 * column, a \X escape as two, and every other code point as one. */
	size_t column = 1;
	bool in_string = false;

	while (p < end) {
		php_json_ctype c = *p;
		if (!in_string) {
			if (c == '"') {
				in_string = true;
			}
			column++;
			p++;
		} else if (c == '"') {
			in_string = false;
			column++;
			p++;
		} else if (c == '\\') {
			if (p + 5 < end && (p[1] | 0x20) == 'u') {
				/* \uXXXX, possibly the high half of a surrogate pair */
				if (php_json_is_hex(p[2], 'd', 'd') && php_json_is_hex(p[3], '8', 'b')
						&& p + 11 < end && p[6] == '\\' && (p[7] | 0x20) == 'u'
						&& php_json_is_hex(p[8], 'd', 'd') && php_json_is_hex(p[9], 'c', 'f')) {
					p += 12;
				} else {
					p += 6;
				}
				column++;
			} else {
				column += 2;
				p += 2;
			}
		} else if ((c & 0xC0) == 0x80) {
			/* UTF-8 continuation byte: counted with its leading byte */
			p++;
		} else {
			column++;
			p++;
		}
	}
	return column;
}

static const php_json_parser_methods default_parser_methods =
{
	php_json_parser_array_create,
	php_json_parser_array_append,
	NULL,
	NULL,
	php_json_parser_object_create,
	php_json_parser_object_update,
	NULL,
	NULL,
};

static const php_json_parser_methods validate_parser_methods =
{
	php_json_parser_array_create_validate,
	php_json_parser_array_append_validate,
	NULL,
	NULL,
	php_json_parser_object_create_validate,
	php_json_parser_object_update_validate,
	NULL,
	NULL,
};

PHP_JSON_API void php_json_parser_init_ex(php_json_parser *parser,
		zval *return_value,
		const char *str,
		size_t str_len,
		int options,
		int max_depth,
		const php_json_parser_methods *parser_methods)
{
	memset(parser, 0, sizeof(php_json_parser));
	php_json_scanner_init(&parser->scanner, str, str_len, options);
	parser->depth = 1;
	parser->max_depth = max_depth;
	parser->return_value = return_value;
	memcpy(&parser->methods, parser_methods, sizeof(php_json_parser_methods));
}

PHP_JSON_API void php_json_parser_init(php_json_parser *parser,
		zval *return_value,
		const char *str,
		size_t str_len,
		int options,
		int max_depth)
{
	php_json_parser_init_ex(
			parser,
			return_value,
			str,
			str_len,
			options,
			max_depth,
			&default_parser_methods);
}

PHP_JSON_API int php_json_parse(php_json_parser *parser)
{
	return php_json_yyparse(parser);
}

const php_json_parser_methods* php_json_get_validate_methods(void)
{
	return &validate_parser_methods;
}
