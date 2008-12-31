%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
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

#define YYSTYPE zval

#ifdef ZTS
#define YYPARSE_PARAM tsrm_ls
#define YYLEX_PARAM tsrm_ls
#endif

#define ZEND_INI_PARSER_CB	(CG(ini_parser_param))->ini_parser_cb
#define ZEND_INI_PARSER_ARG	(CG(ini_parser_param))->arg

int ini_lex(zval *ini_lval TSRMLS_DC);
#ifdef ZTS
int ini_parse(void *arg);
#else
int ini_parse(void);
#endif

zval yylval;

#ifndef ZTS
extern int ini_lex(zval *ini_lval TSRMLS_DC);
extern FILE *ini_in;
extern void init_cfg_scanner(void);
#endif

void zend_ini_do_op(char type, zval *result, zval *op1, zval *op2)
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

void zend_ini_init_string(zval *result)
{
	Z_STRVAL_P(result) = malloc(1);
	Z_STRVAL_P(result)[0] = 0;
	Z_STRLEN_P(result) = 0;
	Z_TYPE_P(result) = IS_STRING;
}

void zend_ini_add_string(zval *result, zval *op1, zval *op2)
{
	int length = Z_STRLEN_P(op1) + Z_STRLEN_P(op2);

	Z_STRVAL_P(result) = (char *) realloc(Z_STRVAL_P(op1), length+1);
	memcpy(Z_STRVAL_P(result)+Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
	Z_STRVAL_P(result)[length] = 0;
	Z_STRLEN_P(result) = length;
	Z_TYPE_P(result) = IS_STRING;
}

void zend_ini_get_constant(zval *result, zval *name)
{
	zval z_constant;
	TSRMLS_FETCH();

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

void zend_ini_get_var(zval *result, zval *name)
{
	zval curval;
	char *envvar;
	TSRMLS_FETCH();

	if (zend_get_configuration_directive(Z_STRVAL_P(name), Z_STRLEN_P(name)+1, &curval) == SUCCESS) {
		Z_STRVAL_P(result) = zend_strndup(Z_STRVAL(curval), Z_STRLEN(curval));
		Z_STRLEN_P(result) = Z_STRLEN(curval);
	} else if ((envvar = zend_getenv(Z_STRVAL_P(name), Z_STRLEN_P(name) TSRMLS_CC)) != NULL ||
			   (envvar = getenv(Z_STRVAL_P(name))) != NULL) {
		Z_STRVAL_P(result) = strdup(envvar);
		Z_STRLEN_P(result) = strlen(envvar);
	} else {
		zend_ini_init_string(result);
	}
}


static void ini_error(char *str)
{
	char *error_buf;
	int error_buf_len;
	char *currently_parsed_filename;
	TSRMLS_FETCH();

	currently_parsed_filename = zend_ini_scanner_get_filename(TSRMLS_C);
	if (currently_parsed_filename) {
		error_buf_len = 128+strlen(currently_parsed_filename); /* should be more than enough */
		error_buf = (char *) emalloc(error_buf_len);

		sprintf(error_buf, "Error parsing %s on line %d\n", currently_parsed_filename, zend_ini_scanner_get_lineno(TSRMLS_C));
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


ZEND_API int zend_parse_ini_file(zend_file_handle *fh, zend_bool unbuffered_errors, zend_ini_parser_cb_t ini_parser_cb, void *arg)
{
	int retval;
	zend_ini_parser_param ini_parser_param;
	TSRMLS_FETCH();

	ini_parser_param.ini_parser_cb = ini_parser_cb;
	ini_parser_param.arg = arg;

	CG(ini_parser_param) = &ini_parser_param;
	if (zend_ini_open_file_for_scanning(fh TSRMLS_CC)==FAILURE) {
		return FAILURE;
	}

	CG(ini_parser_unbuffered_errors) = unbuffered_errors;
	retval = ini_parse(TSRMLS_C);

	zend_ini_close_file(fh TSRMLS_CC);

	if (retval==0) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


ZEND_API int zend_parse_ini_string(char *str, zend_bool unbuffered_errors, zend_ini_parser_cb_t ini_parser_cb, void *arg)
{
	zend_ini_parser_param ini_parser_param;
	TSRMLS_FETCH();

	ini_parser_param.ini_parser_cb = ini_parser_cb;
	ini_parser_param.arg = arg;

	CG(ini_parser_param) = &ini_parser_param;
	if (zend_ini_prepare_string_for_scanning(str TSRMLS_CC)==FAILURE) {
		return FAILURE;
	}

	CG(ini_parser_unbuffered_errors) = unbuffered_errors;

	if (ini_parse(TSRMLS_C)) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


%}

%pure_parser
%token TC_STRING
%token TC_ENCAPSULATED_STRING
%token BRACK
%token SECTION
%token CFG_TRUE
%token CFG_FALSE
%token TC_DOLLAR_CURLY
%left '|' '&'
%right '~' '!'

%%

statement_list:
		statement_list statement
	|	/* empty */
;

statement:
		TC_STRING '=' string_or_value {
#if DEBUG_CFG_PARSER
			printf("'%s' = '%s'\n", Z_STRVAL($1), Z_STRVAL($3));
#endif
			ZEND_INI_PARSER_CB(&$1, &$3, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG);
			free(Z_STRVAL($1));
			free(Z_STRVAL($3));
		}
	|	TC_STRING BRACK '=' string_or_value {
#if DEBUG_CFG_PARSER
			printf("'%s'[ ] = '%s'\n", Z_STRVAL($1), Z_STRVAL($4));
#endif
			ZEND_INI_PARSER_CB(&$1, &$4, ZEND_INI_PARSER_POP_ENTRY, ZEND_INI_PARSER_ARG);
			free(Z_STRVAL($1));
			free(Z_STRVAL($4));
		}
	|	TC_STRING { ZEND_INI_PARSER_CB(&$1, NULL, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG); free(Z_STRVAL($1)); }
	|	SECTION { ZEND_INI_PARSER_CB(&$1, NULL, ZEND_INI_PARSER_SECTION, ZEND_INI_PARSER_ARG); free(Z_STRVAL($1)); }
	|	'\n'
;


string_or_value:
		expr { $$ = $1; }
	|	CFG_TRUE { $$ = $1; }
	|	CFG_FALSE { $$ = $1; }
	|	'\n' { zend_ini_init_string(&$$); }
	|	/* empty */ { zend_ini_init_string(&$$); }
;


var_string_list:
		cfg_var_ref { $$ = $1; }
	|	TC_ENCAPSULATED_STRING { $$ = $1; }
	|	constant_string { $$ = $1; }
	|	var_string_list cfg_var_ref { zend_ini_add_string(&$$, &$1, &$2); free($2.value.str.val); }
	|	var_string_list TC_ENCAPSULATED_STRING { zend_ini_add_string(&$$, &$1, &$2); free(Z_STRVAL($2)); }
	|	var_string_list constant_string { zend_ini_add_string(&$$, &$1, &$2); free($2.value.str.val); }
;

cfg_var_ref:
		TC_DOLLAR_CURLY TC_STRING '}' { zend_ini_get_var(&$$, &$2); free($2.value.str.val); }
;

expr:
		var_string_list			{ $$ = $1; }
	|	expr '|' expr			{ zend_ini_do_op('|', &$$, &$1, &$3); }
	|	expr '&' expr			{ zend_ini_do_op('&', &$$, &$1, &$3); }
	|	'~' expr				{ zend_ini_do_op('~', &$$, &$2, NULL); }
	|	'!'	expr				{ zend_ini_do_op('!', &$$, &$2, NULL); }
	|	'(' expr ')'			{ $$ = $2; }
;

constant_string:
		TC_STRING { zend_ini_get_constant(&$$, &$1); }
;

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
