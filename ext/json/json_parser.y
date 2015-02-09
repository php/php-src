%code top {
/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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

#define PHP_JSON_USE(uv) ((void) (uv))
#define PHP_JSON_USE_1(uvr, uv1) PHP_JSON_USE(uvr); PHP_JSON_USE(uv1)
#define PHP_JSON_USE_2(uvr, uv1, uv2) PHP_JSON_USE(uvr); PHP_JSON_USE(uv1); PHP_JSON_USE(uv2)

}

%pure-parser
%name-prefix "php_json_yy"
%lex-param  { php_json_parser *parser  }
%parse-param { php_json_parser *parser }

%union {
	zval value;
	struct {
		zval key;
		zval val;
	} pair;
}


%token <value> PHP_JSON_T_NUL
%token <value> PHP_JSON_T_TRUE
%token <value> PHP_JSON_T_FALSE
%token <value> PHP_JSON_T_INT
%token <value> PHP_JSON_T_DOUBLE
%token <value> PHP_JSON_T_STRING
%token <value> PHP_JSON_T_ESTRING
%token <value> PHP_JSON_T_EOI
%token <value> PHP_JSON_T_ERROR

%type <value> start object key value array errlex
%type <value> members member elements element
%type <pair> pair

%destructor { zval_dtor(&$$); } <value>
%destructor { zend_hash_destroy($$); FREE_HASHTABLE($$); } <ht>
%destructor { zval_dtor(&$$.key); zval_dtor(&$$.val); } <pair>

%code {
int php_json_yylex(union YYSTYPE *value, php_json_parser *parser);
void php_json_yyerror(php_json_parser *parser, char const *msg);
void php_json_parser_object_init(php_json_parser *parser, zval *object);
void php_json_parser_object_update(php_json_parser *parser, zval *object, zval *zkey, zval *zvalue);
void php_json_parser_array_init(zval *object);
void php_json_parser_array_append(zval *array, zval *zvalue);

#define PHP_JSON_DEPTH_DEC --parser->depth
#define PHP_JSON_DEPTH_INC \
	if (parser->max_depth && parser->depth >= parser->max_depth) { \
		parser->scanner.errcode = PHP_JSON_ERROR_DEPTH; \
		YYERROR; \
	} \
	++parser->depth
}

%% /* Rules */

start:
		value PHP_JSON_T_EOI    { $$ = $1; ZVAL_COPY_VALUE(parser->return_value, &$1); PHP_JSON_USE($2); YYACCEPT; }
	|	value errlex            { PHP_JSON_USE_2($$, $1, $2); }
;

object:
		'{' { PHP_JSON_DEPTH_INC; } members object_end { PHP_JSON_DEPTH_DEC; $$ = $3; }
;

object_end:
		'}'
	|	']'                     { parser->scanner.errcode = PHP_JSON_ERROR_STATE_MISMATCH; YYERROR; }
;

members:
		/* empty */             { php_json_parser_object_init(parser, &$$); }
	|	member
;

member:
		pair                    { php_json_parser_object_init(parser, &$$); php_json_parser_object_update(parser, &$$, &$1.key, &$1.val); }
	|	member ',' pair         { php_json_parser_object_update(parser, &$1, &$3.key, &$3.val); $$ = $1; }
	|	member errlex           { PHP_JSON_USE_2($$, $1, $2); }
;

pair:
		key ':' value           { $$.key = $1; $$.val = $3; }
	|	key errlex              { PHP_JSON_USE_2($$, $1, $2); }
;

array:
		'[' { PHP_JSON_DEPTH_INC; } elements array_end { PHP_JSON_DEPTH_DEC; $$ = $3; }
;

array_end:
		']'
	|	'}'                     { parser->scanner.errcode = PHP_JSON_ERROR_STATE_MISMATCH; YYERROR; }
;

elements:
		/* empty */             { php_json_parser_array_init(&$$); }
	|	element
;

element:
		value                   { php_json_parser_array_init(&$$); php_json_parser_array_append(&$$, &$1); }
	|	element ',' value       { php_json_parser_array_append(&$1, &$3); $$ = $1; }
	|	element errlex          { PHP_JSON_USE_2($$, $1, $2); }
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
	|	errlex
;

errlex:
		PHP_JSON_T_ERROR             { PHP_JSON_USE_1($$, $1); YYERROR; }
;
	
%% /* Functions */

void php_json_parser_init(php_json_parser *parser, zval *return_value, char *str, size_t str_len, int options, int max_depth)
{
	memset(parser, 0, sizeof(php_json_parser));
	php_json_scanner_init(&parser->scanner, str, str_len, options);
	parser->depth = 1;
	parser->max_depth = max_depth;
	parser->return_value = return_value;
}

php_json_error_code php_json_parser_error_code(php_json_parser *parser)
{
	return parser->scanner.errcode;
}

void php_json_parser_object_init(php_json_parser *parser, zval *object)
{
	if (parser->scanner.options & PHP_JSON_OBJECT_AS_ARRAY) {
		array_init(object);
	} else {
		object_init(object);
	}
}

void php_json_parser_object_update(php_json_parser *parser, zval *object, zval *zkey, zval *zvalue)
{
	char *key = Z_STRVAL_P(zkey);
	int key_len = Z_STRLEN_P(zkey);

	if (parser->scanner.options & PHP_JSON_OBJECT_AS_ARRAY) {
		add_assoc_zval_ex(object, key, key_len, zvalue);
	} else {
		if (key_len == 0) {
			key = "_empty_";
			key_len = sizeof("_empty_") - 1;
		}
		add_property_zval_ex(object, key, key_len, zvalue TSRMLS_CC);

		if (Z_REFCOUNTED_P(zvalue)) {
			Z_DELREF_P(zvalue);
		}
	}
	zval_dtor(zkey);
}

void php_json_parser_array_init(zval *array)
{
	array_init(array);
}

void php_json_parser_array_append(zval *array, zval *zvalue)
{
	add_next_index_zval(array, zvalue);
}
	
int php_json_yylex(union YYSTYPE *value, php_json_parser *parser)
{
	int token = php_json_scan(&parser->scanner);
	value->value = parser->scanner.value;
	return token;
}

void php_json_yyerror(php_json_parser *parser, char const *msg)
{
	if (!parser->scanner.errcode) {
		parser->scanner.errcode = PHP_JSON_ERROR_SYNTAX;
	}
}
