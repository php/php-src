%code top {
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
	HashTable *ht;
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
%type <ht> members member elements element
%type <pair> pair

%destructor { zval_dtor(&$$); } <value>
%destructor { zend_hash_destroy($$); FREE_HASHTABLE($$); } <ht>
%destructor { zval_dtor(&$$.key); zval_dtor(&$$.val); } <pair>

%code {
int php_json_yylex(union YYSTYPE *value, php_json_parser *parser);
void php_json_yyerror(php_json_parser *parser, char const *msg);
void php_json_parser_object_to_zval(php_json_parser *parser, zval *zv, HashTable *ht);
void php_json_parser_array_to_zval(zval *zv, HashTable *ht);
void php_json_parser_ht_init(HashTable **ht, uint nSize);
void php_json_parser_ht_update(php_json_parser *parser, HashTable *ht, zval *zkey, zval *zvalue);
void php_json_parser_ht_append(HashTable *ht, zval *zvalue);

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
		value PHP_JSON_T_EOI    { $$ = $1; ZVAL_DUP(parser->return_value, &$1); PHP_JSON_USE($2); YYACCEPT; }
	|	value errlex            { PHP_JSON_USE_2($$, $1, $2); }
;

object:
		'{' { PHP_JSON_DEPTH_INC; } members object_end { PHP_JSON_DEPTH_DEC; php_json_parser_object_to_zval(parser, &$$, $3); }
;

object_end:
		'}'
	|	']'                     { parser->scanner.errcode = PHP_JSON_ERROR_STATE_MISMATCH; YYERROR; }
;

members:
		/* empty */             { php_json_parser_ht_init(&$$, 0); }
	|	member
;

member:
		pair                    { php_json_parser_ht_init(&$$, 4); php_json_parser_ht_update(parser, $$, &$1.key, &$1.val); }
	|	member ',' pair         { php_json_parser_ht_update(parser, $1, &$3.key, &$3.val); $$ = $1; }
	|	member errlex           { PHP_JSON_USE_2($$, $1, $2); }
;

pair:
		key ':' value           { $$.key = $1; $$.val = $3; }
	|	key errlex              { PHP_JSON_USE_2($$, $1, $2); }
;

array:
		'[' { PHP_JSON_DEPTH_INC; } elements array_end { PHP_JSON_DEPTH_DEC; php_json_parser_array_to_zval(&$$, $3); }
;

array_end:
		']'
	|	'}'                     { parser->scanner.errcode = PHP_JSON_ERROR_STATE_MISMATCH; YYERROR; }
;

elements:
		/* empty */             { php_json_parser_ht_init(&$$, 0); }
	|	element
;

element:
		value                   { php_json_parser_ht_init(&$$, 4); php_json_parser_ht_append($$, &$1); }
	|	element ',' value       { php_json_parser_ht_append($1, &$3); $$ = $1; }
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

void php_json_parser_init(php_json_parser *parser, zval *return_value, char *str, int str_len, long options, long max_depth TSRMLS_DC)
{
	memset(parser, 0, sizeof(php_json_parser));
	php_json_scanner_init(&parser->scanner, str, str_len, options);
	parser->depth = 1;
	parser->max_depth = max_depth;
	parser->return_value = return_value;
	TSRMLS_SET_CTX(parser->zts_ctx);
}

php_json_error_code php_json_parser_error_code(php_json_parser *parser)
{
	return parser->scanner.errcode;
}

void php_json_parser_object_to_zval(php_json_parser *parser, zval *zv, HashTable *ht)
{
	TSRMLS_FETCH_FROM_CTX(parser->zts_ctx);
	
	if (parser->scanner.options & PHP_JSON_OBJECT_AS_ARRAY) {
		php_json_parser_array_to_zval(zv, ht);
	} else {
		object_and_properties_init(zv, zend_standard_class_def, ht);
	}
}

void php_json_parser_array_to_zval(zval *zv, HashTable *ht)
{
	Z_TYPE_P(zv) = IS_ARRAY;
	Z_ARRVAL_P(zv) = ht;
}

void php_json_parser_ht_init(HashTable **ht, uint nSize)
{
	ALLOC_HASHTABLE(*ht);
	zend_hash_init(*ht, nSize, NULL, ZVAL_PTR_DTOR, 0);
}

void php_json_parser_ht_update(php_json_parser *parser, HashTable *ht, zval *zkey, zval *zvalue)
{
	zval *data;
	char *key = Z_STRVAL_P(zkey);
	int key_len = Z_STRLEN_P(zkey)+1;
	MAKE_STD_ZVAL(data);
	ZVAL_ZVAL(data, zvalue, 0, 0);
	
	if (parser->scanner.options & PHP_JSON_OBJECT_AS_ARRAY) {
		zend_symtable_update(ht, key, key_len, &data, sizeof(zval *), NULL);
	} else {
		if (key_len == 1) {
			key = "_empty_";
			key_len = sizeof("_empty_");
		}
		zend_hash_update(ht, key, key_len, &data, sizeof(zval *), NULL);
	}
	
	zval_dtor(zkey);
}

void php_json_parser_ht_append(HashTable *ht, zval *zvalue)
{
	zval *data;
	MAKE_STD_ZVAL(data);
	ZVAL_ZVAL(data, zvalue, 0, 0);
	zend_hash_next_index_insert(ht, &data, sizeof(zval *), NULL);
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
