%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
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
#if 0
#include "ext/standard/dl.h"
#include "ext/standard/file.h"
#include "ext/standard/php_browscap.h"
#endif
#include "zend_extensions.h"


#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
//#include "win32/wfile.h"
#endif

#define YYSTYPE zval

#define PARSING_MODE_CFG		0
#define PARSING_MODE_BROWSCAP	1
#define PARSING_MODE_STANDALONE	2

static HashTable configuration_hash;
extern HashTable browser_hash;
ZEND_API extern char *php_ini_path;
static HashTable *active_hash_table;
static zval *current_section;
static char *currently_parsed_filename;

static int parsing_mode;

zval yylval;

extern int cfglex(zval *cfglval);
extern FILE *cfgin;
extern int cfglineno;
extern void init_cfg_scanner(void);

zval *cfg_get_entry(char *name, uint name_length)
{
	zval *tmp;

	if (zend_hash_find(&configuration_hash, name, name_length, (void **) &tmp)==SUCCESS) {
		return tmp;
	} else {
		return NULL;
	}
}


ZEND_API int cfg_get_long(char *varname,long *result)
{
	zval *tmp,var;
	
	if (zend_hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=(long)NULL;
		return FAILURE;
	}
	var = *tmp;
	zval_copy_ctor(&var);
	convert_to_long(&var);
	*result = var.value.lval;
	return SUCCESS;
}


ZEND_API int cfg_get_double(char *varname,double *result)
{
	zval *tmp,var;
	
	if (zend_hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=(double)0;
		return FAILURE;
	}
	var = *tmp;
	zval_copy_ctor(&var);
	convert_to_double(&var);
	*result = var.value.dval;
	return SUCCESS;
}


ZEND_API int cfg_get_string(char *varname, char **result)
{
	zval *tmp;

	if (zend_hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=NULL;
		return FAILURE;
	}
	*result = tmp->value.str.val;
	return SUCCESS;
}


static void yyerror(char *str)
{
	char *error_buf;
	int error_buf_len;

	error_buf_len = 128+strlen(currently_parsed_filename); /* should be more than enough */
	error_buf = (char *) emalloc(error_buf_len);
	
	sprintf(error_buf, "Error parsing %s on line %d\n", currently_parsed_filename, cfglineno);
#ifdef PHP_WIN32
	MessageBox(NULL, error_buf, "PHP Error", MB_OK|MB_TOPMOST|0x00200000L);
#else
	fprintf(stderr, "PHP:  %s", error_buf);
#endif
	efree(error_buf);
}


static void pvalue_config_destructor(zval *pvalue)
{
	if (pvalue->type == IS_STRING && pvalue->value.str.val != empty_string) {
		free(pvalue->value.str.val);
	}
}


static void pvalue_browscap_destructor(zval *pvalue)
{
	if (pvalue->type == IS_OBJECT || pvalue->type == IS_ARRAY) {
		zend_hash_destroy(pvalue->value.obj.properties);
		free(pvalue->value.obj.properties);
	}
}


#if 0
int php_init_config(void)
{
	PLS_FETCH();

	if (zend_hash_init(&configuration_hash, 0, NULL, (dtor_func_t) pvalue_config_destructor, 1)==FAILURE) {
		return FAILURE;
	}

#if USE_CONFIG_FILE
	{
		char *env_location,*default_location,*php_ini_search_path;
		int safe_mode_state = PG(safe_mode);
		char *open_basedir = PG(open_basedir);
		char *opened_path;
		int free_default_location=0;
		
		env_location = getenv("PHPRC");
		if (!env_location) {
			env_location="";
		}
#ifdef PHP_WIN32
		{
			if (php_ini_path) {
				default_location = php_ini_path;
			} else {
				default_location = (char *) malloc(512);
			
				if (!GetWindowsDirectory(default_location,255)) {
					default_location[0]=0;
				}
				free_default_location=1;
			}
		}
#else
		if (!php_ini_path) {
			default_location = CONFIGURATION_FILE_PATH;
		} else {
			default_location = php_ini_path;
		}
#endif

/* build a path */
		php_ini_search_path = (char *) malloc(sizeof(".")+strlen(env_location)+strlen(default_location)+2+1);

		if (!php_ini_path) {
#ifdef PHP_WIN32
			sprintf(php_ini_search_path,".;%s;%s",env_location,default_location);
#else
			sprintf(php_ini_search_path,".:%s:%s",env_location,default_location);
#endif
		} else {
			/* if path was set via -c flag, only look there */
			strcpy(php_ini_search_path,default_location);
		}
		PG(safe_mode) = 0;
		PG(open_basedir) = NULL;
		cfgin = php_fopen_with_path("php.ini","r",php_ini_search_path,&opened_path);
		free(php_ini_search_path);
		if (free_default_location) {
			free(default_location);
		}
		PG(safe_mode) = safe_mode_state;
		PG(open_basedir) = open_basedir;

		if (!cfgin) {
			return SUCCESS;  /* having no configuration file is ok */
		}

		if (opened_path) {
			zval tmp;
			
			tmp.value.str.val = strdup(opened_path);
			tmp.value.str.len = strlen(opened_path);
			tmp.type = IS_STRING;
			zend_hash_update(&configuration_hash,"cfg_file_path",sizeof("cfg_file_path"),(void *) &tmp,sizeof(zval),NULL);
#if DEBUG_CFG_PARSER
			php_printf("INI file opened at '%s'\n",opened_path);
#endif
			efree(opened_path);
		}
			
		init_cfg_scanner();
		active_hash_table = &configuration_hash;
		parsing_mode = PARSING_MODE_CFG;
		currently_parsed_filename = "php.ini";
		yyparse();
		fclose(cfgin);
	}
	
#endif
	
	return SUCCESS;
}


PHP_MINIT_FUNCTION(browscap)
{
	char *browscap = INI_STR("browscap");

	if (browscap) {
		if (zend_hash_init(&browser_hash, 0, NULL, (dtor_func_t) pvalue_browscap_destructor, 1)==FAILURE) {
			return FAILURE;
		}

		cfgin = V_FOPEN(browscap, "r");
		if (!cfgin) {
			php_error(E_WARNING,"Cannot open '%s' for reading", browscap);
			return FAILURE;
		}
		init_cfg_scanner();
		active_hash_table = &browser_hash;
		parsing_mode = PARSING_MODE_BROWSCAP;
		currently_parsed_filename = browscap;
		yyparse();
		fclose(cfgin);
	}

	return SUCCESS;
}

/* {{{ proto void parse_ini_file(string filename)
   Parse configuration file */
ZEND_FUNCTION(parse_ini_file)
{
#ifdef ZTS
	php_error(E_WARNING, "parse_ini_file() is not supported in multithreaded PHP");
	RETURN_FALSE;
#else
	zval **filename;

	if (ARG_COUNT(ht)!=1 || zend_get_parameters_ex(1, &filename)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(filename);
	cfgin = V_FOPEN((*filename)->value.str.val, "r");
	if (!cfgin) {
		php_error(E_WARNING,"Cannot open '%s' for reading", (*filename)->value.str.val);
		return;
	}
	array_init(return_value);
	init_cfg_scanner();
	active_hash_table = return_value->value.ht;
	parsing_mode = PARSING_MODE_STANDALONE;
	currently_parsed_filename = (*filename)->value.str.val;
	yyparse();
	fclose(cfgin);
#endif
}
/* }}} */


int php_shutdown_config(void)
{
	zend_hash_destroy(&configuration_hash);
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(browscap)
{
	if (INI_STR("browscap")) {
		zend_hash_destroy(&browser_hash);
	}
	return SUCCESS;
}

#endif


static void convert_browscap_pattern(zval *pattern)
{
	register int i,j;
	char *t;

	for (i=0; i<pattern->value.str.len; i++) {
		if (pattern->value.str.val[i]=='*' || pattern->value.str.val[i]=='?' || pattern->value.str.val[i]=='.') {
			break;
		}
	}

	if (i==pattern->value.str.len) { /* no wildcards */
		pattern->value.str.val = zend_strndup(pattern->value.str.val, pattern->value.str.len);
		return;
	}

	t = (char *) malloc(pattern->value.str.len*2);
	
	for (i=0,j=0; i<pattern->value.str.len; i++,j++) {
		switch (pattern->value.str.val[i]) {
			case '?':
				t[j] = '.';
				break;
			case '*':
				t[j++] = '.';
				t[j] = '*';
				break;
			case '.':
				t[j++] = '\\';
				t[j] = '.';
				break;
			default:
				t[j] = pattern->value.str.val[i];
				break;
		}
	}
	t[j]=0;
	pattern->value.str.val = t;
	pattern->value.str.len = j;
}


void do_cfg_op(char type, zval *result, zval *op1, zval *op2)
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


void do_cfg_get_constant(zval *result, zval *name)
{
	zval z_constant;

	if (zend_get_constant(name->value.str.val, name->value.str.len, &z_constant)) {
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


%}

%pure_parser
%token TC_STRING
%token TC_ENCAPSULATED_STRING
%token SECTION
%token CFG_TRUE
%token CFG_FALSE
%token EXTENSION
%token T_ZEND_EXTENSION
%token T_ZEND_EXTENSION_TS
%token T_ZEND_EXTENSION_DEBUG
%token T_ZEND_EXTENSION_DEBUG_TS
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
			printf("'%s' = '%s'\n",$1.value.str.val,$3.value.str.val);
#endif
			$3.type = IS_STRING;
			switch (parsing_mode) {
				case PARSING_MODE_CFG:
					zend_hash_update(active_hash_table, $1.value.str.val, $1.value.str.len+1, &$3, sizeof(zval), NULL);
					if (active_hash_table == &configuration_hash) {
						zend_alter_ini_entry($1.value.str.val, $1.value.str.len+1, $3.value.str.val, $3.value.str.len+1, ZEND_INI_SYSTEM, ZEND_INI_STAGE_STARTUP);
					}
					break;
				case PARSING_MODE_BROWSCAP:
					if (current_section) {
						zval *new_property;
						char *new_key;

						new_property = (zval *) malloc(sizeof(zval));
						INIT_PZVAL(new_property);
						new_property->value.str.val = $3.value.str.val;
						new_property->value.str.len = $3.value.str.len;
						new_property->type = IS_STRING;
						
						new_key = zend_strndup($1.value.str.val, $1.value.str.len);
						zend_str_tolower(new_key,$1.value.str.len);
						zend_hash_update(current_section->value.obj.properties, new_key, $1.value.str.len+1, &new_property, sizeof(zval *), NULL);
						free(new_key);
					}
					break;
				case PARSING_MODE_STANDALONE: {
						zval *entry;

						MAKE_STD_ZVAL(entry);
						entry->value.str.val = estrndup($3.value.str.val, $3.value.str.len);
						entry->value.str.len = $3.value.str.len;
						entry->type = IS_STRING;
						zend_hash_update(active_hash_table, $1.value.str.val, $1.value.str.len+1, &entry, sizeof(zval *), NULL);
						pvalue_config_destructor(&$3);
					}
					break;
			}		
			free($1.value.str.val);
		}
	|	TC_STRING { free($1.value.str.val); }
	|	EXTENSION '=' cfg_string {
			if (parsing_mode==PARSING_MODE_CFG) {
				zval dummy;

#if DEBUG_CFG_PARSER
				printf("Loading '%s'\n",$3.value.str.val);
#endif
				php_dl(&$3,MODULE_PERSISTENT,&dummy);
			}
		}
	|	T_ZEND_EXTENSION '=' cfg_string {
			if (parsing_mode==PARSING_MODE_CFG) {
#if !defined(ZTS) && !ZEND_DEBUG
				zend_load_extension($3.value.str.val);
#endif
				free($3.value.str.val);
			}
		}
	|	T_ZEND_EXTENSION_TS '=' cfg_string { 
			if (parsing_mode==PARSING_MODE_CFG) {
#if defined(ZTS) && !ZEND_DEBUG
				zend_load_extension($3.value.str.val);
#endif
				free($3.value.str.val);
			}
		}
	|	T_ZEND_EXTENSION_DEBUG '=' cfg_string { 
			if (parsing_mode==PARSING_MODE_CFG) {
#if !defined(ZTS) && ZEND_DEBUG
				zend_load_extension($3.value.str.val);
#endif
				free($3.value.str.val);
			}
		}
	|	T_ZEND_EXTENSION_DEBUG_TS '=' cfg_string { 
			if (parsing_mode==PARSING_MODE_CFG) {
#if defined(ZTS) && ZEND_DEBUG
				zend_load_extension($3.value.str.val);
#endif
				free($3.value.str.val);
			}
		}
	|	SECTION { 
			if (parsing_mode==PARSING_MODE_BROWSCAP) {
				zval *processed;

				/*printf("'%s' (%d)\n",$1.value.str.val,$1.value.str.len+1);*/
				current_section = (zval *) malloc(sizeof(zval));
				INIT_PZVAL(current_section);
				processed = (zval *) malloc(sizeof(zval));
				INIT_PZVAL(processed);

				current_section->value.obj.ce = &zend_standard_class_def;
				current_section->value.obj.properties = (HashTable *) malloc(sizeof(HashTable));
				current_section->type = IS_OBJECT;
				zend_hash_init(current_section->value.obj.properties, 0, NULL, (dtor_func_t) pvalue_config_destructor, 1);
				zend_hash_update(active_hash_table, $1.value.str.val, $1.value.str.len+1, (void *) &current_section, sizeof(zval *), NULL);

				processed->value.str.val = $1.value.str.val;
				processed->value.str.len = $1.value.str.len;
				processed->type = IS_STRING;
				convert_browscap_pattern(processed);
				zend_hash_update(current_section->value.obj.properties, "browser_name_pattern", sizeof("browser_name_pattern"), (void *) &processed, sizeof(zval *), NULL);
			}
			free($1.value.str.val);
		}
	|	'\n'
;


cfg_string:
		TC_STRING { $$ = $1; }
	|	TC_ENCAPSULATED_STRING { $$ = $1; }
;

string_or_value:
		expr { $$ = $1; }
	|	TC_ENCAPSULATED_STRING { $$ = $1; }
	|	CFG_TRUE { $$ = $1; }
	|	CFG_FALSE { $$ = $1; }
	|	'\n' { $$.value.str.val = strdup(""); $$.value.str.len=0; $$.type = IS_STRING; }
	|	'\0' { $$.value.str.val = strdup(""); $$.value.str.len=0; $$.type = IS_STRING; }
;

expr:
		constant_string			{ $$ = $1; }
	|	expr '|' expr			{ do_cfg_op('|', &$$, &$1, &$3); }
	|	expr '&' expr			{ do_cfg_op('&', &$$, &$1, &$3); }
	|	'~' expr				{ do_cfg_op('~', &$$, &$2, NULL); }
	|	'!'	expr				{ do_cfg_op('!', &$$, &$2, NULL); }
	|	'(' expr ')'			{ $$ = $2; }
;

constant_string:
		TC_STRING { do_cfg_get_constant(&$$, &$1); }
;
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
