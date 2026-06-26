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

%locations
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
static int php_json_yylex(union YYSTYPE *value, YYLTYPE *location, php_json_parser *parser);
static void php_json_yyerror(YYLTYPE *location, php_json_parser *parser, char const *msg);
static int php_json_parser_array_create(php_json_parser *parser, zval *array);
static int php_json_parser_object_create(php_json_parser *parser, zval *array);
static void php_json_merge_array_into(zval *dest, zval *src);
static void php_json_merge_object_into(zval *dest, zval *src);
static void php_json_merge_values(zval *dest, zval *src);
static void php_json_build_duplicate_value_array(zval *existing, zval *zvalue, zval *result);
static int php_json_object_update_array_merge(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue);
static int php_json_object_update_object_merge(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue);
static int php_json_object_update_array_collect(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue);
static int php_json_object_update_object_collect(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue);
static int php_json_parser_object_update_merge(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue);
static int php_json_parser_object_update_array(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue);

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

static void php_json_merge_array_into(zval *dest, zval *src)
{
	HashTable *dest_ht = Z_ARRVAL_P(dest);
	HashTable *src_ht = Z_ARRVAL_P(src);
	zend_string *key;
	zval *val;

	ZEND_HASH_FOREACH_STR_KEY_VAL(src_ht, key, val) {
		if (key) {
			zval *existing = zend_symtable_find(dest_ht, key);

			if (existing
				&& Z_TYPE_P(existing) == IS_ARRAY && Z_TYPE_P(val) == IS_ARRAY
			) {
				if (zend_array_is_list(Z_ARRVAL_P(existing)) && zend_array_is_list(Z_ARRVAL_P(val))) {
					zval *elem;

					ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(val), elem) {
						zval copy;

						ZVAL_COPY(&copy, elem);
						zend_hash_next_index_insert(Z_ARRVAL_P(existing), &copy);
					} ZEND_HASH_FOREACH_END();
				} else {
					php_json_merge_array_into(existing, val);
				}
			} else if (existing
				&& Z_TYPE_P(existing) == IS_OBJECT && Z_TYPE_P(val) == IS_OBJECT
			) {
				php_json_merge_object_into(existing, val);
			} else {
				zval copy;

				ZVAL_COPY(&copy, val);
				zend_symtable_update(dest_ht, key, &copy);
			}
		} else {
			zval copy;

			ZVAL_COPY(&copy, val);
			zend_hash_next_index_insert(dest_ht, &copy);
		}
	} ZEND_HASH_FOREACH_END();
}

static void php_json_merge_object_into(zval *dest, zval *src)
{
	zend_string *key;
	zval *val;

	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_OBJPROP_P(src), key, val) {
		if (!key) {
			continue;
		}

		zval *existing = zend_read_property_ex(Z_OBJCE_P(dest), Z_OBJ_P(dest), key, 1, NULL);

		if (existing && Z_TYPE_P(existing) != IS_UNDEF) {
			if (Z_TYPE_P(existing) == IS_ARRAY && Z_TYPE_P(val) == IS_ARRAY) {
				if (zend_array_is_list(Z_ARRVAL_P(existing)) && zend_array_is_list(Z_ARRVAL_P(val))) {
					zval *elem;

					ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(val), elem) {
						zval copy;

						ZVAL_COPY(&copy, elem);
						zend_hash_next_index_insert(Z_ARRVAL_P(existing), &copy);
					} ZEND_HASH_FOREACH_END();
				} else {
					php_json_merge_array_into(existing, val);
				}
				continue;
			}

			if (Z_TYPE_P(existing) == IS_OBJECT && Z_TYPE_P(val) == IS_OBJECT) {
				php_json_merge_object_into(existing, val);
				continue;
			}
		}

		zval copy;

		ZVAL_COPY(&copy, val);
		zend_std_write_property(Z_OBJ_P(dest), key, &copy, NULL);
		zval_ptr_dtor(&copy);
	} ZEND_HASH_FOREACH_END();
}

static void php_json_merge_values(zval *dest, zval *src)
{
	if (Z_TYPE_P(dest) == IS_ARRAY && Z_TYPE_P(src) == IS_ARRAY) {
		php_json_merge_array_into(dest, src);
	} else if (Z_TYPE_P(dest) == IS_OBJECT && Z_TYPE_P(src) == IS_OBJECT) {
		php_json_merge_object_into(dest, src);
	}
}

static void php_json_build_duplicate_value_array(zval *existing, zval *zvalue, zval *result)
{
	array_init(result);
	zend_hash_next_index_insert(Z_ARRVAL_P(result), existing);
	zend_hash_next_index_insert(Z_ARRVAL_P(result), zvalue);
}

static int php_json_object_update_array_merge(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	HashTable *ht = Z_ARRVAL_P(object);
	zval *existing = zend_symtable_find(ht, key);

	(void) parser;

	if (!existing) {
		zend_symtable_update(ht, key, zvalue);
		return SUCCESS;
	}

	if ((Z_TYPE_P(existing) == IS_ARRAY && Z_TYPE_P(zvalue) == IS_ARRAY)
		|| (Z_TYPE_P(existing) == IS_OBJECT && Z_TYPE_P(zvalue) == IS_OBJECT)
	) {
		php_json_merge_values(existing, zvalue);
		zval_ptr_dtor_nogc(zvalue);
		return SUCCESS;
	}

	zend_symtable_update(ht, key, zvalue);
	return SUCCESS;
}

static int php_json_object_update_object_merge(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	zval *existing = zend_read_property_ex(Z_OBJCE_P(object), Z_OBJ_P(object), key, 1, NULL);

	(void) parser;

	if (!existing || Z_TYPE_P(existing) == IS_UNDEF) {
		zend_std_write_property(Z_OBJ_P(object), key, zvalue, NULL);
		Z_TRY_DELREF_P(zvalue);
		return SUCCESS;
	}

	if ((Z_TYPE_P(existing) == IS_ARRAY && Z_TYPE_P(zvalue) == IS_ARRAY)
		|| (Z_TYPE_P(existing) == IS_OBJECT && Z_TYPE_P(zvalue) == IS_OBJECT)
	) {
		php_json_merge_values(existing, zvalue);
		zval_ptr_dtor_nogc(zvalue);
		return SUCCESS;
	}

	zend_std_write_property(Z_OBJ_P(object), key, zvalue, NULL);
	Z_TRY_DELREF_P(zvalue);
	return SUCCESS;
}

static int php_json_object_update_array_collect(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	HashTable *ht = Z_ARRVAL_P(object);
	zval *existing = zend_symtable_find(ht, key);

	(void) parser;

	if (!existing) {
		zend_symtable_update(ht, key, zvalue);
		return SUCCESS;
	}

	if (Z_TYPE_P(existing) == IS_ARRAY && zend_array_is_list(Z_ARRVAL_P(existing))) {
		zend_hash_next_index_insert(Z_ARRVAL_P(existing), zvalue);
		return SUCCESS;
	}

	zval collected, copy;

	ZVAL_COPY(&copy, existing);
	php_json_build_duplicate_value_array(&copy, zvalue, &collected);
	zend_symtable_update(ht, key, &collected);
	return SUCCESS;
}

static int php_json_object_update_object_collect(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	zval *existing = zend_read_property_ex(Z_OBJCE_P(object), Z_OBJ_P(object), key, 1, NULL);

	(void) parser;

	if (!existing || Z_TYPE_P(existing) == IS_UNDEF) {
		zend_std_write_property(Z_OBJ_P(object), key, zvalue, NULL);
		Z_TRY_DELREF_P(zvalue);
		return SUCCESS;
	}

	if (Z_TYPE_P(existing) == IS_ARRAY && zend_array_is_list(Z_ARRVAL_P(existing))) {
		zval copy;

		ZVAL_COPY(&copy, zvalue);
		zend_hash_next_index_insert(Z_ARRVAL_P(existing), &copy);
		zval_ptr_dtor_nogc(zvalue);
		return SUCCESS;
	}

	zval collected, copy;

	ZVAL_COPY(&copy, existing);
	php_json_build_duplicate_value_array(&copy, zvalue, &collected);
	zend_std_write_property(Z_OBJ_P(object), key, &collected, NULL);
	zval_ptr_dtor(&collected);
	zval_ptr_dtor_nogc(zvalue);
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

static int php_json_parser_object_update_merge(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	if (Z_TYPE_P(object) == IS_ARRAY) {
		if (FAILURE == php_json_object_update_array_merge(parser, object, key, zvalue)) {
			zend_string_release_ex(key, 0);
			return FAILURE;
		}
	} else {
		if (ZSTR_LEN(key) > 0 && ZSTR_VAL(key)[0] == '\0') {
			parser->scanner.errcode = PHP_JSON_ERROR_INVALID_PROPERTY_NAME;
			zend_string_release_ex(key, 0);
			zval_ptr_dtor_nogc(zvalue);
			zval_ptr_dtor_nogc(object);
			return FAILURE;
		}
		if (FAILURE == php_json_object_update_object_merge(parser, object, key, zvalue)) {
			zend_string_release_ex(key, 0);
			return FAILURE;
		}
	}
	zend_string_release_ex(key, 0);

	return SUCCESS;
}

static int php_json_parser_object_update_array(php_json_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	if (Z_TYPE_P(object) == IS_ARRAY) {
		if (FAILURE == php_json_object_update_array_collect(parser, object, key, zvalue)) {
			zend_string_release_ex(key, 0);
			return FAILURE;
		}
	} else {
		if (ZSTR_LEN(key) > 0 && ZSTR_VAL(key)[0] == '\0') {
			parser->scanner.errcode = PHP_JSON_ERROR_INVALID_PROPERTY_NAME;
			zend_string_release_ex(key, 0);
			zval_ptr_dtor_nogc(zvalue);
			zval_ptr_dtor_nogc(object);
			return FAILURE;
		}
		if (FAILURE == php_json_object_update_object_collect(parser, object, key, zvalue)) {
			zend_string_release_ex(key, 0);
			return FAILURE;
		}
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

static int php_json_yylex(union YYSTYPE *value, YYLTYPE *location, php_json_parser *parser)
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

	location->first_column = PHP_JSON_SCANNER_LOCATION(parser->scanner, first_column);
	location->first_line = PHP_JSON_SCANNER_LOCATION(parser->scanner, first_line);
	location->last_column = PHP_JSON_SCANNER_LOCATION(parser->scanner, last_column);
	location->last_line = PHP_JSON_SCANNER_LOCATION(parser->scanner, last_line);

	return token;
}

static void php_json_yyerror(YYLTYPE *location, php_json_parser *parser, char const *msg)
{
	if (!parser->scanner.errcode) {
		parser->scanner.errcode = PHP_JSON_ERROR_SYNTAX;
	}
}

PHP_JSON_API php_json_error_code php_json_parser_error_code(const php_json_parser *parser)
{
	return parser->scanner.errcode;
}

PHP_JSON_API void php_json_parser_error_details(const php_json_parser *parser, php_json_error_details *out)
{
	out->code = parser->scanner.errcode;
	out->line = parser->scanner.errloc.first_line;
	out->column = parser->scanner.errloc.first_column;
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

static const php_json_parser_methods merge_parser_methods =
{
	php_json_parser_array_create,
	php_json_parser_array_append,
	NULL,
	NULL,
	php_json_parser_object_create,
	php_json_parser_object_update_merge,
	NULL,
	NULL,
};

static const php_json_parser_methods array_parser_methods =
{
	php_json_parser_array_create,
	php_json_parser_array_append,
	NULL,
	NULL,
	php_json_parser_object_create,
	php_json_parser_object_update_array,
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
	const php_json_parser_methods *parser_methods = &default_parser_methods;

	if (options & PHP_JSON_DUPLICATE_KEY_MERGE) {
		parser_methods = &merge_parser_methods;
	} else if (options & PHP_JSON_DUPLICATE_KEY_ARRAY) {
		parser_methods = &array_parser_methods;
	}

	php_json_parser_init_ex(
			parser,
			return_value,
			str,
			str_len,
			options,
			max_depth,
			parser_methods);
}

PHP_JSON_API int php_json_parse(php_json_parser *parser)
{
	return php_json_yyparse(parser);
}

const php_json_parser_methods* php_json_get_validate_methods(void)
{
	return &validate_parser_methods;
}
