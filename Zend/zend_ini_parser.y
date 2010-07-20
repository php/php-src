%include{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2010 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author:  Pierrick Charron <pierrick@php.net>                         |
   | Yacc version authors:                                                |
   |          Zeev Suraski <zeev@zend.com>                                |
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

#define NDEBUG
#define YYNOERRORRECOVERY

#define ZEND_INI_PARSER_CB	(CG(ini_parser_param))->ini_parser_cb
#define ZEND_INI_PARSER_ARG	(CG(ini_parser_param))->arg
#define ZEND_INI_PARSER_SE 	(CG(ini_parser_param))->syntax_error

int ini_parse(TSRMLS_D);
void *zend_ini_parseAlloc(void *(*mallocProc)(size_t ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC));
void zend_ini_parseFree(void *p, void (*freeProc)(void* ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC));
void zend_ini_parse(void *yyp, int yymajor, zval yyminor TSRMLS_DC);

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
	Z_STRVAL_P(result) = (char *) malloc(1);
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
		Z_TYPE_P(result) = IS_STRING;
	/* ..or if not found, try ENV */
	} else if ((envvar = zend_getenv(Z_STRVAL_P(name), Z_STRLEN_P(name) TSRMLS_CC)) != NULL ||
			   (envvar = getenv(Z_STRVAL_P(name))) != NULL) {
		Z_STRVAL_P(result) = strdup(envvar);
		Z_STRLEN_P(result) = strlen(envvar);
		Z_TYPE_P(result) = IS_STRING;
	} else {
		zend_ini_init_string(result);
	}
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
	ini_parser_param.syntax_error = 0;
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
	ini_parser_param.syntax_error = 0;
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

int ini_parse(TSRMLS_D) /* {{{ */
{
	int token;
	void *pParser = zend_ini_parseAlloc(malloc);

	if (pParser == NULL) {
		zend_ini_parseFree(pParser, free);
		return 1;
	}

	while (1) {
		zval zendval;
		token = ini_lex(&zendval TSRMLS_CC);
		zend_ini_parse(pParser, token, zendval TSRMLS_CC);

		if (token == 0 || ZEND_INI_PARSER_SE) {
			break;
		}
	}

	zend_ini_parseFree(pParser, free);
	if (ZEND_INI_PARSER_SE) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

}

%syntax_error {
	char *error_buf;
	int error_buf_len;
	char *currently_parsed_filename;

	ZEND_INI_PARSER_SE = 1;

	currently_parsed_filename = zend_ini_scanner_get_filename(TSRMLS_C);
	if (currently_parsed_filename) {
		error_buf_len = 128 + INI_SCNG(yy_leng) + strlen(currently_parsed_filename); /* should be more than enough */
		error_buf = (char *) emalloc(error_buf_len);
		sprintf(error_buf, "syntax error, unexpected '%.*s' in %s on line %d\n", INI_SCNG(yy_leng), INI_SCNG(yy_text), currently_parsed_filename, zend_ini_scanner_get_lineno(TSRMLS_C));
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

%token_type {zval}

%name zend_ini_parse

%ifdef ZTS
	%extra_argument {void ***tsrm_ls}
%endif

%token_prefix TC_

/* Precedences */
%left BW_OR BW_AND.
%right BW_NOT BOOL_NOT.

start ::= statement_list.

statement_list ::= statement_list statement.
statement_list ::= .

statement ::= SECTION section_string_or_value(C) RBRACKET. {
#if DEBUG_CFG_PARSER
			printf("SECTION: [%s]\n", Z_STRVAL(C));
#endif
			ZEND_INI_PARSER_CB(&C, NULL, NULL, ZEND_INI_PARSER_SECTION, ZEND_INI_PARSER_ARG TSRMLS_CC);
			free(Z_STRVAL(C));
}
statement ::= LABEL(B) EQUAL string_or_value(D). {
#if DEBUG_CFG_PARSER
			printf("NORMAL: '%s' = '%s'\n", Z_STRVAL(B), Z_STRVAL(D));
#endif
			ZEND_INI_PARSER_CB(&B, &D, NULL, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG TSRMLS_CC);
			free(Z_STRVAL(B));
			free(Z_STRVAL(D));
}
statement ::= OFFSET(B) option_offset(C) RBRACKET EQUAL string_or_value(F). {
#if DEBUG_CFG_PARSER
			printf("OFFSET: '%s'[%s] = '%s'\n", Z_STRVAL(B), Z_STRVAL(C), Z_STRVAL(F));
#endif
			ZEND_INI_PARSER_CB(&B, &F, &C, ZEND_INI_PARSER_POP_ENTRY, ZEND_INI_PARSER_ARG TSRMLS_CC);
			free(Z_STRVAL(B));
			free(Z_STRVAL(C));
			free(Z_STRVAL(F));
}
statement ::= LABEL(B). { ZEND_INI_PARSER_CB(&B, NULL, NULL, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG TSRMLS_CC); free(Z_STRVAL(B)); }
statement ::= END_OF_LINE.

section_string_or_value(A) ::= var_string_list_section(B). { A = B; }
section_string_or_value(A) ::= . { zend_ini_init_string(&A); }

string_or_value(A) ::= expr(B). { A = B; }
string_or_value(A) ::= BOOL_TRUE(B). { A = B; }
string_or_value(A) ::= BOOL_FALSE(B). { A = B; }
string_or_value(A) ::= END_OF_LINE. { zend_ini_init_string(&A); }

option_offset(A) ::= var_string_list(B). { A = B; }
option_offset(A) ::= . { zend_ini_init_string(&A); }

encapsed_list(A) ::= encapsed_list(B) cfg_var_ref(C). { zend_ini_add_string(&A, &B, &C); free(Z_STRVAL(C)); }
encapsed_list(A) ::= encapsed_list(B) QUOTED_STRING(C). { zend_ini_add_string(&A, &B, &C); free(Z_STRVAL(C)); }
encapsed_list(A) ::= . { zend_ini_init_string(&A); }

var_string_list_section(A) ::= cfg_var_ref(B). { A = B; }
var_string_list_section(A) ::= constant_literal(B). { A = B; }
var_string_list_section(A) ::= QUOTE encapsed_list(C) QUOTE. { A = C; }
var_string_list_section(A) ::= var_string_list_section(B) cfg_var_ref(C). { zend_ini_add_string(&A, &B, &C); free(Z_STRVAL(C)); }
var_string_list_section(A) ::= var_string_list_section(B) constant_literal(C). { zend_ini_add_string(&A, &B, &C); free(Z_STRVAL(C)); }
var_string_list_section(A) ::= var_string_list_section(B) QUOTE encapsed_list(D) QUOTE. { zend_ini_add_string(&A, &B, &D); free(Z_STRVAL(D)); }

var_string_list(A) ::= cfg_var_ref(B). { A = B; }
var_string_list(A) ::= constant_string(B). { A = B; }
var_string_list(A) ::= QUOTE encapsed_list(C) QUOTE. { A = C; }
var_string_list(A) ::= var_string_list(B) cfg_var_ref(C). { zend_ini_add_string(&A, &B, &C); free(Z_STRVAL(C)); }
var_string_list(A) ::= var_string_list(B) constant_string(C). { zend_ini_add_string(&A, &B, &C); free(Z_STRVAL(C)); }
var_string_list(A) ::= var_string_list(B) QUOTE encapsed_list(D) QUOTE. { zend_ini_add_string(&A, &B, &D); free(Z_STRVAL(D)); }

expr(A) ::= var_string_list(B).		{ A = B; }
expr(A) ::= expr(B) BW_OR expr(D).	{ zend_ini_do_op('|', &A, &B, &D); }
expr(A) ::= expr(B) BW_AND expr(D).	{ zend_ini_do_op('&', &A, &B, &D); }
expr(A) ::= BW_NOT expr(C).			{ zend_ini_do_op('~', &A, &C, NULL); }
expr(A) ::= BOOL_NOT expr(C).		{ zend_ini_do_op('!', &A, &C, NULL); }
expr(A) ::= LPAREN expr(C) RPAREN.	{ A = C; }

cfg_var_ref(A) ::= DOLLAR_CURLY VARNAME(C) RBRACE. { zend_ini_get_var(&A, &C TSRMLS_CC); free(Z_STRVAL(C)); }

constant_literal(A) ::= CONSTANT(B).	{ A = B; }
constant_literal(A) ::= RAW(B).			{ A = B; }
constant_literal(A) ::= NUMBER(B).		{ A = B; }
constant_literal(A) ::= STRING(B).		{ A = B; }
constant_literal(A) ::= WHITESPACE(B).	{ A = B; }

constant_string(A) ::= CONSTANT(B).		{ zend_ini_get_constant(&A, &B TSRMLS_CC); }
constant_string(A) ::= RAW(B).			{ A = B; }
constant_string(A) ::= NUMBER(B).		{ A = B; }
constant_string(A) ::= STRING(B).		{ A = B; }
constant_string(A) ::= WHITESPACE(B).	{ A = B; }

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
