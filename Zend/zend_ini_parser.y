%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2006 Zend Technologies Ltd. (http://www.zend.com) |
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

	i_op1 = atoi(op1->value.str.val);
	free(op1->value.str.val);
	if (op2) {
		i_op2 = atoi(op2->value.str.val);
		free(op2->value.str.val);
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

	result->value.str.len = zend_sprintf(str_result, "%d", i_result);
	result->value.str.val = (char *) malloc(result->value.str.len+1);
	memcpy(result->value.str.val, str_result, result->value.str.len);
	result->value.str.val[result->value.str.len] = 0;
	result->type = IS_STRING;
}

void zend_ini_init_string(zval *result)
{
	result->value.str.val = malloc(1);
	result->value.str.val[0] = 0;
	result->value.str.len = 0;
	result->type = IS_STRING;
}

void zend_ini_add_string(zval *result, zval *op1, zval *op2)
{           
    int length = op1->value.str.len + op2->value.str.len;

	result->value.str.val = (char *) realloc(op1->value.str.val, length+1);
    memcpy(result->value.str.val+op1->value.str.len, op2->value.str.val, op2->value.str.len);
    result->value.str.val[length] = 0;
    result->value.str.len = length;
    result->type = IS_STRING;
}

void zend_ini_get_constant(zval *result, zval *name)
{
	zval z_constant;
	TSRMLS_FETCH();

	if (!memchr(name->value.str.val, ':', name->value.str.len)
		   	&& zend_get_constant(name->value.str.val, name->value.str.len, &z_constant TSRMLS_CC)) {
		/* z_constant is emalloc()'d */
		convert_to_string(&z_constant);
		result->value.str.val = zend_strndup(z_constant.value.str.val, z_constant.value.str.len);
		result->value.str.len = z_constant.value.str.len;
		result->type = z_constant.type;
		zval_dtor(&z_constant);
		free(name->value.str.val);	
	} else {
		*result = *name;
	}
}

void zend_ini_get_var(zval *result, zval *name)
{
	zval curval;
	char *envvar;
	TSRMLS_FETCH();

	if (zend_get_configuration_directive(name->value.str.val, name->value.str.len+1, &curval) == SUCCESS) {
		result->value.str.val = zend_strndup(curval.value.str.val, curval.value.str.len);
		result->value.str.len = curval.value.str.len;
	} else if ((envvar = zend_getenv(name->value.str.val, name->value.str.len TSRMLS_CC)) != NULL ||
			   (envvar = getenv(name->value.str.val)) != NULL) {
		result->value.str.val = strdup(envvar);
		result->value.str.len = strlen(envvar);
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
	error_buf_len = 128+strlen(currently_parsed_filename); /* should be more than enough */
	error_buf = (char *) emalloc(error_buf_len);

	sprintf(error_buf, "Error parsing %s on line %d\n", currently_parsed_filename, zend_ini_scanner_get_lineno(TSRMLS_C));

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
			printf("'%s' = '%s'\n", $1.value.str.val, $3.value.str.val);
#endif
			ZEND_INI_PARSER_CB(&$1, &$3, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG);
			free($1.value.str.val);
			free($3.value.str.val);
		}
	|	TC_STRING BRACK '=' string_or_value {
#if DEBUG_CFG_PARSER
			printf("'%s'[ ] = '%s'\n", $1.value.str.val, $4.value.str.val);
#endif
			ZEND_INI_PARSER_CB(&$1, &$4, ZEND_INI_PARSER_POP_ENTRY, ZEND_INI_PARSER_ARG);
			free($1.value.str.val);
			free($4.value.str.val);
		}
	|	TC_STRING { ZEND_INI_PARSER_CB(&$1, NULL, ZEND_INI_PARSER_ENTRY, ZEND_INI_PARSER_ARG); free($1.value.str.val); }
	|	SECTION { ZEND_INI_PARSER_CB(&$1, NULL, ZEND_INI_PARSER_SECTION, ZEND_INI_PARSER_ARG); free($1.value.str.val); }
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
	|	var_string_list TC_ENCAPSULATED_STRING { zend_ini_add_string(&$$, &$1, &$2); free($2.value.str.val); }
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
