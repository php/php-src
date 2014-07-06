%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Jani Taskinen <jani@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define DEBUG_CFG_PARSER 0

#include "zend.h"
#include "zend_API.h"
#include "zend_ini.h"
#include "zend_constants.h"
#include "zend_ini_scanner.h"
#include "zend_extensions.h"

#define YYERROR_VERBOSE
#define YYSTYPE zval

#ifdef ZTS
#define YYPARSE_PARAM tsrm_ls
#define YYLEX_PARAM tsrm_ls
int ini_parse(void *arg);
#else
int ini_parse(void);
#endif

#define ZEND_INI_PARSER_CB	(CG(ini_parser_param))->ini_parser_cb
#define ZEND_INI_PARSER_ARG	(CG(ini_parser_param))->arg

/* {{{ zend_ini_do_op()
*/
static void zend_ini_do_op(char type, zval *result, zval *op1, zval *op2)
{
	int i_result;
	int i_op1, i_op2;
	char str_result[MAX_LENGTH_OF_LONG];

	i_op1 = atoi(Z_STRVAL_P(op1));
	free(Z_STRVAL_P(op1));
	if (op2) {
		i_op2 = atoi(Z_STRVAL_P(op2));
		free(Z_STRVAL_P(op2));
	} else {
		i_op2 = 0;
	}

	switch (type) {
		case '|':
			i_result = i_op1 | i_op2;
			break;
		case '&':
			i_result = i_op1 & i_op2;
			break;
		case '^':
			i_result = i_op1 ^ i_op2;
			break;
		case '~':
			i_result = ~i_op1;
			break;
		case '!':
			i_result = !i_op1;
			break;
		default:
			i_result = 0;
			break;
	}

	Z_STRLEN_P(result) = zend_sprintf(str_result, "%d", i_result);
	Z_STRVAL_P(result) = (char *) malloc(Z_STRLEN_P(result)+1);
	memcpy(Z_STRVAL_P(result), str_result, Z_STRLEN_P(result));
	Z_STRVAL_P(result)[Z_STRLEN_P(result)] = 0;
	Z_TYPE_P(result) = IS_STRING;
}
/* }}} */

/* {{{ zend_ini_init_string()
*/
static void zend_ini_init_string(zval *result)
{
	Z_STRVAL_P(result) = malloc(1);
	Z_STRVAL_P(result)[0] = 0;
	Z_STRLEN_P(result) = 0;
	Z_TYPE_P(result) = IS_STRING;
}
/* }}} */

/* {{{ zend_ini_add_string()
*/
static void zend_ini_add_string(zval *result, zval *op1, zval *op2)
{
	int length = Z_STRLEN_P(op1) + Z_STRLEN_P(op2);

	Z_STRVAL_P(result) = (char *) realloc(Z_STRVAL_P(op1), length+1);
	memcpy(Z_STRVAL_P(result)+Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
	Z_STRVAL_P(result)[length] = 0;
	Z_STRLEN_P(result) = length;
	Z_TYPE_P(result) = IS_STRING;
}
/* }}} */

/* {{{ zend_ini_get_constant()
*/
static void zend_ini_get_constant(zval *result, zval *name TSRMLS_DC)
{
	zval z_constant;

	/* If name contains ':' it is not a constant. Bug #26893. */
	if (!memchr(Z_STRVAL_P(name), ':', Z_STRLEN_P(name))
		   	&& zend_get_constant(Z_STRVAL_P(name), Z_STRLEN_P(name), &z_constant TSRMLS_CC)) {
		/* z_constant is emalloc()'d */
		convert_to_string(&z_constant);
		Z_STRVAL_P(result) = zend_strndup(Z_STRVAL(z_constant), Z_STRLEN(z_constant));
		Z_STRLEN_P(result) = Z_STRLEN(z_constant);
		Z_TYPE_P(result) = Z_TYPE(z_constant);
		zval_dtor(&z_constant);
		free(Z_STRVAL_P(name));
	} else {
		*result = *name;
	}
}
/* }}} */

/* {{{ zend_ini_get_var()
*/
static void zend_ini_get_var(zval *result, zval *name TSRMLS_DC)
{
	zval curval;
	char *envvar;

	/* Fetch configuration option value */
	if (zend_get_configuration_directive(Z_STRVAL_P(name), Z_STRLEN_P(name)+1, &curval) == SUCCESS) {
		Z_STRVAL_P(result) = zend_strndup(Z_STRVAL(curval), Z_STRLEN(curval));
		Z_STRLEN_P(result) = Z_STRLEN(curval);
	/* ..or if not found, try ENV */
	} else if ((envvar = zend_getenv(Z_STRVAL_P(name), Z_STRLEN_P(name) TSRMLS_CC)) != NULL ||
			   (envvar = getenv(Z_STRVAL_P(name))) != NULL) {
		Z_STRVAL_P(result) = strdup(envvar);
		Z_STRLEN_P(result) = strlen(envvar);
	} else {
		zend_ini_init_string(result);
	}
}
/* }}} */

/* {{{ ini_error()
*/
static void ini_error(char *msg)
{
	char *error_buf;
	int error_buf_len;
	char *currently_parsed_filename;
	TSRMLS_FETCH();

	currently_parsed_filename = zend_ini_scanner_get_filename(TSRMLS_C);
	if (currently_parsed_filename) {
		error_buf_len = 128 + strlen(msg) + strlen(currently_parsed_filename); /* should be more than enough */
		error_buf = (char *) emalloc(error_buf_len);

		sprintf(error_buf, "%s in %s on line %d\n", msg, currently_parsed_filename, zend_ini_scanner_get_lineno(TSRMLS_C));
	} else {
		error_buf = estrdup("Invalid configuration directive\n");
	}

	if (CG(ini_parser_unbuffered_errors)) {
#ifdef PHP_WIN32
		MessageBox(NULL, error_buf, "PHP Error", MB_OK|MB_TOPMOST|0x00200000L);
#else
		fprintf(stderr, "PHP:  %s", error_buf);
#endif
	} else {
		zend_error(E_WARNING, "%s", error_buf);
	}
	efree(error_buf);
}
/* }}} */

/* {{{ zend_parse_ini_file()
*/
ZEND_API int zend_parse_ini_file(zend_file_handle *fh, zend_bool unbuffered_errors, int scanner_mode, zend_ini_parser_cb_t ini_parser_cb, void *arg TSRMLS_DC)
{
	int retval;
	zend_ini_parser_param ini_parser_param;

	ini_parser_param.ini_parser_cb = ini_parser_cb;
	ini_parser_param.arg = arg;
	CG(ini_parser_param) = &ini_parser_param;

	if (zend_ini_open_file_for_scanning(fh, scanner_mode TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	CG(ini_parser_unbuffered_errors) = unbuffered_errors;
	retval = ini_parse(TSRMLS_C);
	zend_file_handle_dtor(fh TSRMLS_CC);

	shutdown_ini_scanner(TSRMLS_C);
	
	if (retval == 0) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/* {{{ zend_parse_ini_string()
*/
ZEND_API int zend_parse_ini_string(char *str, zend_bool unbuffered_errors, int scanner_mode, zend_ini_parser_cb_t ini_parser_cb, void *arg TSRMLS_DC)
{
	int retval;
	zend_ini_parser_param ini_parser_param;

	ini_parser_param.ini_parser_cb = ini_parser_cb;
	ini_parser_param.arg = arg;
	CG(ini_parser_param) = &ini_parser_param;

	if (zend_ini_prepare_string_for_scanning(str, scanner_mode TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	CG(ini_parser_unbuffered_errors) = unbuffered_errors;
	retval = ini_parse(TSRMLS_C);

	shutdown_ini_scanner(TSRMLS_C);

	if (retval == 0) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

%}

%expect 0
%pure_parser

%token TC_SECTION
%token TC_RAW
%token TC_CONSTANT
%token TC_NUMBER
%token TC_STRING
%token TC_WHITESPACE
%token TC_LABEL
%token TC_OFFSET
%token TC_DOLLAR_CURLY
%token TC_VARNAME
%token TC_QUOTED_STRING
%token BOOL_TRUE
%token BOOL_FALSE
%token END_OF_LINE
%token '=' ':' ',' '.' '"' '\'' '^' '+' '-' '/' '*' '%' '$' '~' '<' '>' '?' '@' '{' '}'
%left '|' '&' '^'
%right '~' '!'

%%

statement_list:
		statement_list statement
	|	/* empty */
;

statement:
		TC_SECTION section_string_or_value ']' {
#if DEBUG_CFG_PARSER
			printf("SECTION: [%s]\n", Z_STRVAL($2));
#endif
			ZEND_INI_PARSER_CB(&$2, NULL, NULL, ZEND_INI_PARSER_SECTION, ZEND_INI_PARSER_ARG TSRMLS_CC);
			free(Z_STRVAL($2));
		}
	|	TC_LABEL '=' string_or_value {
#if DEBUG_CFG_PARSER
			printf("NORMAL: '%s' = '%s'\n", Z_STRVAL($1), Z_STRVAL($3));
#endif
			ZEND_INI_PARSER_CB(&$1, &$3, NULL, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG TSRMLS_CC);
			free(Z_STRVAL($1));
			free(Z_STRVAL($3));
		}
	|	TC_OFFSET option_offset ']' '=' string_or_value {
#if DEBUG_CFG_PARSER
			printf("OFFSET: '%s'[%s] = '%s'\n", Z_STRVAL($1), Z_STRVAL($2), Z_STRVAL($5));
#endif
			ZEND_INI_PARSER_CB(&$1, &$5, &$2, ZEND_INI_PARSER_POP_ENTRY, ZEND_INI_PARSER_ARG TSRMLS_CC);
			free(Z_STRVAL($1));
			free(Z_STRVAL($2));
			free(Z_STRVAL($5));
		}
	|	TC_LABEL	{ ZEND_INI_PARSER_CB(&$1, NULL, NULL, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG TSRMLS_CC); free(Z_STRVAL($1)); }
	|	END_OF_LINE
;

section_string_or_value:
		var_string_list_section			{ $$ = $1; }
	|	/* empty */						{ zend_ini_init_string(&$$); }
;

string_or_value:
		expr							{ $$ = $1; }
	|	BOOL_TRUE						{ $$ = $1; }
	|	BOOL_FALSE						{ $$ = $1; }
	|	END_OF_LINE						{ zend_ini_init_string(&$$); }
;

option_offset:
		var_string_list					{ $$ = $1; }
	|	/* empty */						{ zend_ini_init_string(&$$); }
;

encapsed_list:
		encapsed_list cfg_var_ref		{ zend_ini_add_string(&$$, &$1, &$2); free(Z_STRVAL($2)); }
	|	encapsed_list TC_QUOTED_STRING	{ zend_ini_add_string(&$$, &$1, &$2); free(Z_STRVAL($2)); }
	|	/* empty */						{ zend_ini_init_string(&$$); }
;

var_string_list_section:
		cfg_var_ref						{ $$ = $1; }
	|	constant_literal				{ $$ = $1; }
	|	'"' encapsed_list '"'			{ $$ = $2; }
	|	var_string_list_section cfg_var_ref 	{ zend_ini_add_string(&$$, &$1, &$2); free(Z_STRVAL($2)); }
	|	var_string_list_section constant_literal	{ zend_ini_add_string(&$$, &$1, &$2); free(Z_STRVAL($2)); }
	|	var_string_list_section '"' encapsed_list '"'  { zend_ini_add_string(&$$, &$1, &$3); free(Z_STRVAL($3)); }
;

var_string_list:
		cfg_var_ref						{ $$ = $1; }
	|	constant_string					{ $$ = $1; }
	|	'"' encapsed_list '"'			{ $$ = $2; }
	|	var_string_list cfg_var_ref 	{ zend_ini_add_string(&$$, &$1, &$2); free(Z_STRVAL($2)); }
	|	var_string_list constant_string	{ zend_ini_add_string(&$$, &$1, &$2); free(Z_STRVAL($2)); }
	|	var_string_list '"' encapsed_list '"'  { zend_ini_add_string(&$$, &$1, &$3); free(Z_STRVAL($3)); }
;

expr:
		var_string_list					{ $$ = $1; }
	|	expr '|' expr					{ zend_ini_do_op('|', &$$, &$1, &$3); }
	|	expr '&' expr					{ zend_ini_do_op('&', &$$, &$1, &$3); }
	|	expr '^' expr					{ zend_ini_do_op('^', &$$, &$1, &$3); }
	|	'~' expr						{ zend_ini_do_op('~', &$$, &$2, NULL); }
	|	'!'	expr						{ zend_ini_do_op('!', &$$, &$2, NULL); }
	|	'(' expr ')'					{ $$ = $2; }
;

cfg_var_ref:
		TC_DOLLAR_CURLY TC_VARNAME '}'	{ zend_ini_get_var(&$$, &$2 TSRMLS_CC); free(Z_STRVAL($2)); }
;

constant_literal:
		TC_CONSTANT						{ $$ = $1; }
	|	TC_RAW							{ $$ = $1; /*printf("TC_RAW: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_NUMBER						{ $$ = $1; /*printf("TC_NUMBER: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_STRING						{ $$ = $1; /*printf("TC_STRING: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_WHITESPACE					{ $$ = $1; /*printf("TC_WHITESPACE: '%s'\n", Z_STRVAL($1));*/ }
;

constant_string:
		TC_CONSTANT						{ zend_ini_get_constant(&$$, &$1 TSRMLS_CC); }
	|	TC_RAW							{ $$ = $1; /*printf("TC_RAW: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_NUMBER						{ $$ = $1; /*printf("TC_NUMBER: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_STRING						{ $$ = $1; /*printf("TC_STRING: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_WHITESPACE					{ $$ = $1; /*printf("TC_WHITESPACE: '%s'\n", Z_STRVAL($1));*/ }
;

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
