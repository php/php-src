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
  | Author: Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_JSON_PARSER_H
#define	PHP_JSON_PARSER_H

#include "php.h"
#include "php_json_scanner.h"

typedef struct _php_json_parser php_json_parser;

typedef int (*php_json_parser_func_array_create_t)(
		php_json_parser *parser, zval *array);
typedef int (*php_json_parser_func_array_append_t)(
		php_json_parser *parser, zval *array, zval *zvalue);
typedef int (*php_json_parser_func_array_start_t)(
		php_json_parser *parser);
typedef int (*php_json_parser_func_array_end_t)(
		php_json_parser *parser, zval *object);
typedef int (*php_json_parser_func_object_create_t)(
		php_json_parser *parser, zval *object);
typedef int (*php_json_parser_func_object_update_t)(
		php_json_parser *parser, zval *object, zend_string *key, zval *zvalue);
typedef int (*php_json_parser_func_object_start_t)(
		php_json_parser *parser);
typedef int (*php_json_parser_func_object_end_t)(
		php_json_parser *parser, zval *object);

typedef struct _php_json_parser_methods {
	php_json_parser_func_array_create_t array_create;
	php_json_parser_func_array_append_t array_append;
	php_json_parser_func_array_start_t array_start;
	php_json_parser_func_array_end_t array_end;
	php_json_parser_func_object_create_t object_create;
	php_json_parser_func_object_update_t object_update;
	php_json_parser_func_object_start_t object_start;
	php_json_parser_func_object_end_t object_end;
} php_json_parser_methods;

struct _php_json_parser {
	php_json_scanner scanner;
	zval *return_value;
	int depth;
	int max_depth;
	php_json_parser_methods methods;
};

PHP_JSON_API void php_json_parser_init_ex(
		php_json_parser *parser,
		zval *return_value,
		const char *str,
		size_t str_len,
		int options,
		int max_depth,
		const php_json_parser_methods *methods);

PHP_JSON_API void php_json_parser_init(
		php_json_parser *parser,
		zval *return_value,
		const char *str,
		size_t str_len,
		int options,
		int max_depth);

PHP_JSON_API php_json_error_code php_json_parser_error_code(const php_json_parser *parser);

PHP_JSON_API int php_json_parse(php_json_parser *parser);

int php_json_yyparse(php_json_parser *parser);

#endif	/* PHP_JSON_PARSER_H */
