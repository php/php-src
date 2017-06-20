%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifdef ZEND_WIN32
#include "win32/syslog.h"
#endif

#define YYERROR_VERBOSE
#define YYSTYPE zval

int ini_parse(void);

#define ZEND_INI_PARSER_CB	(CG(ini_parser_param))->ini_parser_cb
#define ZEND_INI_PARSER_ARG	(CG(ini_parser_param))->arg

#ifdef _MSC_VER
#define YYMALLOC malloc
#define YYFREE free
#endif

#define ZEND_SYSTEM_INI CG(ini_parser_unbuffered_errors)

/* {{{ zend_ini_do_op()
*/
static void zend_ini_do_op(char type, zval *result, zval *op1, zval *op2)
{
	int i_result;
	int i_op1, i_op2;
	int str_len;
	char str_result[MAX_LENGTH_OF_LONG+1];

	i_op1 = atoi(Z_STRVAL_P(op1));
	zend_string_free(Z_STR_P(op1));
	if (op2) {
		i_op2 = atoi(Z_STRVAL_P(op2));
		zend_string_free(Z_STR_P(op2));
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

	str_len = zend_sprintf(str_result, "%d", i_result);
	ZVAL_NEW_STR(result, zend_string_init(str_result, str_len, ZEND_SYSTEM_INI));
}
/* }}} */

/* {{{ zend_ini_init_string()
*/
static void zend_ini_init_string(zval *result)
{
	if (ZEND_SYSTEM_INI) {
		ZVAL_EMPTY_PSTRING(result);
	} else {
		ZVAL_EMPTY_STRING(result);
	}
}
/* }}} */

/* {{{ zend_ini_add_string()
*/
static void zend_ini_add_string(zval *result, zval *op1, zval *op2)
{
	int length, op1_len;

	if (Z_TYPE_P(op1) != IS_STRING) {
		zend_string *str = zval_get_string(op1);
		/* ZEND_ASSERT(!Z_REFCOUNTED_P(op1)); */
		if (ZEND_SYSTEM_INI) {
			ZVAL_PSTRINGL(op1, ZSTR_VAL(str), ZSTR_LEN(str));
			zend_string_release(str);
		} else {
			ZVAL_STR(op1, str);
		}
	}
	op1_len = (int)Z_STRLEN_P(op1);
	
	if (Z_TYPE_P(op2) != IS_STRING) {
		convert_to_string(op2);
	}
	length = op1_len + (int)Z_STRLEN_P(op2);

	ZVAL_NEW_STR(result, zend_string_extend(Z_STR_P(op1), length, ZEND_SYSTEM_INI));
	memcpy(Z_STRVAL_P(result) + op1_len, Z_STRVAL_P(op2), Z_STRLEN_P(op2) + 1);
}
/* }}} */

/* {{{ zend_ini_get_constant()
*/
static void zend_ini_get_constant(zval *result, zval *name)
{
	zval *c, tmp;

	/* If name contains ':' it is not a constant. Bug #26893. */
	if (!memchr(Z_STRVAL_P(name), ':', Z_STRLEN_P(name))
		   	&& (c = zend_get_constant(Z_STR_P(name))) != 0) {
		if (Z_TYPE_P(c) != IS_STRING) {
			ZVAL_COPY_VALUE(&tmp, c);
			if (Z_OPT_CONSTANT(tmp)) {
				zval_update_constant_ex(&tmp, 1, NULL);
			}
			zval_opt_copy_ctor(&tmp);
			convert_to_string(&tmp);
			c = &tmp;
		}
		ZVAL_NEW_STR(result, zend_string_init(Z_STRVAL_P(c), Z_STRLEN_P(c), ZEND_SYSTEM_INI));
		if (c == &tmp) {
			zend_string_release(Z_STR(tmp));
		}
		zend_string_free(Z_STR_P(name));
	} else {
		*result = *name;
	}
}
/* }}} */

/* {{{ zend_ini_get_var()
*/
static void zend_ini_get_var(zval *result, zval *name)
{
	zval *curval;
	char *envvar;

	/* Fetch configuration option value */
	if ((curval = zend_get_configuration_directive(Z_STR_P(name))) != NULL) {
		ZVAL_NEW_STR(result, zend_string_init(Z_STRVAL_P(curval), Z_STRLEN_P(curval), ZEND_SYSTEM_INI));
	/* ..or if not found, try ENV */
	} else if ((envvar = zend_getenv(Z_STRVAL_P(name), Z_STRLEN_P(name))) != NULL ||
			   (envvar = getenv(Z_STRVAL_P(name))) != NULL) {
		ZVAL_NEW_STR(result, zend_string_init(envvar, strlen(envvar), ZEND_SYSTEM_INI));
	} else {
		zend_ini_init_string(result);
	}
}
/* }}} */

/* {{{ ini_error()
*/
static ZEND_COLD void ini_error(const char *msg)
{
	char *error_buf;
	int error_buf_len;
	char *currently_parsed_filename;

	currently_parsed_filename = zend_ini_scanner_get_filename();
	if (currently_parsed_filename) {
		error_buf_len = 128 + (int)strlen(msg) + (int)strlen(currently_parsed_filename); /* should be more than enough */
		error_buf = (char *) emalloc(error_buf_len);

		sprintf(error_buf, "%s in %s on line %d\n", msg, currently_parsed_filename, zend_ini_scanner_get_lineno());
	} else {
		error_buf = estrdup("Invalid configuration directive\n");
	}

	if (CG(ini_parser_unbuffered_errors)) {
#ifdef ZEND_WIN32
		syslog(LOG_ALERT, "PHP: %s (%s)", error_buf, GetCommandLine());
#endif
		fprintf(stderr, "PHP:  %s", error_buf);
	} else {
		zend_error(E_WARNING, "%s", error_buf);
	}
	efree(error_buf);
}
/* }}} */

/* {{{ zend_parse_ini_file()
*/
ZEND_API int zend_parse_ini_file(zend_file_handle *fh, zend_bool unbuffered_errors, int scanner_mode, zend_ini_parser_cb_t ini_parser_cb, void *arg)
{
	int retval;
	zend_ini_parser_param ini_parser_param;

	ini_parser_param.ini_parser_cb = ini_parser_cb;
	ini_parser_param.arg = arg;
	CG(ini_parser_param) = &ini_parser_param;

	if (zend_ini_open_file_for_scanning(fh, scanner_mode) == FAILURE) {
		return FAILURE;
	}

	CG(ini_parser_unbuffered_errors) = unbuffered_errors;
	retval = ini_parse();
	zend_file_handle_dtor(fh);

	shutdown_ini_scanner();

	if (retval == 0) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/* {{{ zend_parse_ini_string()
*/
ZEND_API int zend_parse_ini_string(char *str, zend_bool unbuffered_errors, int scanner_mode, zend_ini_parser_cb_t ini_parser_cb, void *arg)
{
	int retval;
	zend_ini_parser_param ini_parser_param;

	ini_parser_param.ini_parser_cb = ini_parser_cb;
	ini_parser_param.arg = arg;
	CG(ini_parser_param) = &ini_parser_param;

	if (zend_ini_prepare_string_for_scanning(str, scanner_mode) == FAILURE) {
		return FAILURE;
	}

	CG(ini_parser_unbuffered_errors) = unbuffered_errors;
	retval = ini_parse();

	shutdown_ini_scanner();

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
%token NULL_NULL
%token END_OF_LINE
%token '=' ':' ',' '.' '"' '\'' '^' '+' '-' '/' '*' '%' '$' '~' '<' '>' '?' '@' '{' '}'
%left '|' '&' '^'
%right '~' '!'

%destructor { zval_ptr_dtor(&$$); } TC_RAW TC_CONSTANT TC_NUMBER TC_STRING TC_WHITESPACE TC_LABEL TC_OFFSET TC_VARNAME BOOL_TRUE BOOL_FALSE NULL_NULL

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
			ZEND_INI_PARSER_CB(&$2, NULL, NULL, ZEND_INI_PARSER_SECTION, ZEND_INI_PARSER_ARG);
			zend_string_release(Z_STR($2));
		}
	|	TC_LABEL '=' string_or_value {
#if DEBUG_CFG_PARSER
			printf("NORMAL: '%s' = '%s'\n", Z_STRVAL($1), Z_STRVAL($3));
#endif
			ZEND_INI_PARSER_CB(&$1, &$3, NULL, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG);
			zend_string_release(Z_STR($1));
			zval_ptr_dtor(&$3);
		}
	|	TC_OFFSET option_offset ']' '=' string_or_value {
#if DEBUG_CFG_PARSER
			printf("OFFSET: '%s'[%s] = '%s'\n", Z_STRVAL($1), Z_STRVAL($2), Z_STRVAL($5));
#endif
			ZEND_INI_PARSER_CB(&$1, &$5, &$2, ZEND_INI_PARSER_POP_ENTRY, ZEND_INI_PARSER_ARG);
			zend_string_release(Z_STR($1));
			if (Z_TYPE($2) == IS_STRING) {
				zend_string_release(Z_STR($2));
			} else {
				zval_dtor(&$2);
			}
			zval_ptr_dtor(&$5);
		}
	|	TC_LABEL	{ ZEND_INI_PARSER_CB(&$1, NULL, NULL, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG); zend_string_release(Z_STR($1)); }
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
	|	NULL_NULL						{ $$ = $1; }
	|	END_OF_LINE						{ zend_ini_init_string(&$$); }
;

option_offset:
		var_string_list					{ $$ = $1; }
	|	/* empty */						{ zend_ini_init_string(&$$); }
;

encapsed_list:
		encapsed_list cfg_var_ref		{ zend_ini_add_string(&$$, &$1, &$2); zend_string_free(Z_STR($2)); }
	|	encapsed_list TC_QUOTED_STRING	{ zend_ini_add_string(&$$, &$1, &$2); zend_string_free(Z_STR($2)); }
	|	/* empty */						{ zend_ini_init_string(&$$); }
;

var_string_list_section:
		cfg_var_ref						{ $$ = $1; }
	|	constant_literal				{ $$ = $1; }
	|	'"' encapsed_list '"'			{ $$ = $2; }
	|	var_string_list_section cfg_var_ref 	{ zend_ini_add_string(&$$, &$1, &$2); zend_string_free(Z_STR($2)); }
	|	var_string_list_section constant_literal	{ zend_ini_add_string(&$$, &$1, &$2); zend_string_free(Z_STR($2)); }
	|	var_string_list_section '"' encapsed_list '"'  { zend_ini_add_string(&$$, &$1, &$3); zend_string_free(Z_STR($3)); }
;

var_string_list:
		cfg_var_ref						{ $$ = $1; }
	|	constant_string					{ $$ = $1; }
	|	'"' encapsed_list '"'			{ $$ = $2; }
	|	var_string_list cfg_var_ref 	{ zend_ini_add_string(&$$, &$1, &$2); zend_string_free(Z_STR($2)); }
	|	var_string_list constant_string	{ zend_ini_add_string(&$$, &$1, &$2); zend_string_free(Z_STR($2)); }
	|	var_string_list '"' encapsed_list '"'  { zend_ini_add_string(&$$, &$1, &$3); zend_string_free(Z_STR($3)); }
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
		TC_DOLLAR_CURLY TC_VARNAME '}'	{ zend_ini_get_var(&$$, &$2); zend_string_free(Z_STR($2)); }
;

constant_literal:
		TC_CONSTANT						{ $$ = $1; }
	|	TC_RAW							{ $$ = $1; /*printf("TC_RAW: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_NUMBER						{ $$ = $1; /*printf("TC_NUMBER: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_STRING						{ $$ = $1; /*printf("TC_STRING: '%s'\n", Z_STRVAL($1));*/ }
	|	TC_WHITESPACE					{ $$ = $1; /*printf("TC_WHITESPACE: '%s'\n", Z_STRVAL($1));*/ }
;

constant_string:
		TC_CONSTANT						{ zend_ini_get_constant(&$$, &$1); }
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
