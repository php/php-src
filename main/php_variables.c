/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdio.h>
#include "php.h"
#include "ext/standard/php_standard.h"
#include "php_variables.h"
#include "php_globals.h"
#include "php_content_types.h"
#include "SAPI.h"

#include "zend_globals.h"


/* for systems that need to override reading of environment variables */
void _php_import_environment_variables(zval *array_ptr TSRMLS_DC);
PHPAPI void (*php_import_environment_variables)(zval *array_ptr TSRMLS_DC) = _php_import_environment_variables;

PHPAPI void php_register_variable(char *var, char *strval, zval *track_vars_array TSRMLS_DC)
{
	php_register_variable_safe(var, strval, strlen(strval), track_vars_array TSRMLS_CC);
}


/* binary-safe version */
PHPAPI void php_register_variable_safe(char *var, char *strval, int str_len, zval *track_vars_array TSRMLS_DC)
{
	zval new_entry;
	assert(strval != NULL);
	
	/* Prepare value */
	Z_STRLEN(new_entry) = str_len;
	if (PG(magic_quotes_gpc)) {
		Z_STRVAL(new_entry) = php_addslashes(strval, Z_STRLEN(new_entry), &Z_STRLEN(new_entry), 0 TSRMLS_CC);
	} else {
		Z_STRVAL(new_entry) = estrndup(strval, Z_STRLEN(new_entry));
	}
	Z_TYPE(new_entry) = IS_STRING;

	php_register_variable_ex(var, &new_entry, track_vars_array TSRMLS_CC);
}


PHPAPI void php_register_variable_ex(char *var, zval *val, pval *track_vars_array TSRMLS_DC)
{
	char *p = NULL;
	char *ip;		/* index pointer */
	char *index;
	int var_len, index_len;
	zval *gpc_element, **gpc_element_p, **top_gpc_p=NULL;
	zend_bool is_array;
	HashTable *symtable1=NULL;
	HashTable *symtable2=NULL;

	assert(var != NULL);
	
	if (track_vars_array) {
		symtable1 = Z_ARRVAL_P(track_vars_array);
	}
	if (PG(register_globals)) {
		if (symtable1) {
			symtable2 = EG(active_symbol_table);
		} else {
			symtable1 = EG(active_symbol_table);
		}	
	}
	if (!symtable1) {
		/* Nothing to do */
		zval_dtor(val);
		return;
	}

	/*
	 * Prepare variable name
	 */
	ip = strchr(var, '[');
	if (ip) {
		is_array = 1;
		*ip = 0;
	} else {
		is_array = 0;
	}
	/* ignore leading spaces in the variable name */
	while (*var && *var==' ') {
		var++;
	}
	var_len = strlen(var);
	if (var_len==0) { /* empty variable name, or variable name with a space in it */
		zval_dtor(val);
		return;
	}
	/* ensure that we don't have spaces or dots in the variable name (not binary safe) */
	for (p=var; *p; p++) {
		switch(*p) {
			case ' ':
			case '.':
				*p='_';
				break;
		}
	}

	index = var;
	index_len = var_len;

	while (1) {
		if (is_array) {
			char *escaped_index = NULL, *index_s;
			int new_idx_len = 0;

			ip++;
			index_s = ip;
			if (isspace(*ip)) {
				ip++;
			}
			if (*ip==']') {
				index_s = NULL;
			} else {
				ip = strchr(ip, ']');
				if (!ip) {
					/* PHP variables cannot contain '[' in their names, so we replace the character with a '_' */
					*(index_s - 1) = '_';
					index_len = var_len = strlen(var);
					goto plain_var;
					return;
				}
				*ip = 0;
				new_idx_len = strlen(index_s);	
			}

			if (!index) {
				MAKE_STD_ZVAL(gpc_element);
				array_init(gpc_element);
				zend_hash_next_index_insert(symtable1, &gpc_element, sizeof(zval *), (void **) &gpc_element_p);
			} else {
				if (PG(magic_quotes_gpc) && (index!=var)) {
					/* no need to addslashes() the index if it's the main variable name */
					escaped_index = php_addslashes(index, index_len, &index_len, 0 TSRMLS_CC);
				} else {
					escaped_index = index;
				}
				if (zend_hash_find(symtable1, escaped_index, index_len+1, (void **) &gpc_element_p)==FAILURE
					|| Z_TYPE_PP(gpc_element_p) != IS_ARRAY) {
					MAKE_STD_ZVAL(gpc_element);
					array_init(gpc_element);
					zend_hash_update(symtable1, escaped_index, index_len+1, &gpc_element, sizeof(zval *), (void **) &gpc_element_p);
				}
				if (index!=escaped_index) {
					efree(escaped_index);
				}
			}
			if (!top_gpc_p) {
				top_gpc_p = gpc_element_p;
			}
			symtable1 = Z_ARRVAL_PP(gpc_element_p);
			/* ip pointed to the '[' character, now obtain the key */
			index = index_s;
			index_len = new_idx_len;

			ip++;
			if (*ip=='[') {
				is_array = 1;
				*ip = 0;
			} else {
				is_array = 0;
			}
		} else {
plain_var:
			MAKE_STD_ZVAL(gpc_element);
			gpc_element->value = val->value;
			Z_TYPE_P(gpc_element) = Z_TYPE_P(val);
			if (!index) {
				zend_hash_next_index_insert(symtable1, &gpc_element, sizeof(zval *), (void **) &gpc_element_p);
			} else {
				zend_hash_update(symtable1, index, index_len+1, &gpc_element, sizeof(zval *), (void **) &gpc_element_p);
			}
			if (!top_gpc_p) {
				top_gpc_p = gpc_element_p;
			}
			break;
		}
	}

	if (top_gpc_p) {
		if (symtable2) {
			zend_hash_update(symtable2, var, var_len+1, top_gpc_p, sizeof(zval *), NULL);
			(*top_gpc_p)->refcount++;
		}	
	}
}


SAPI_API SAPI_POST_HANDLER_FUNC(php_std_post_handler)
{
	char *var, *val;
	char *strtok_buf = NULL;
	zval *array_ptr = (zval *) arg;

	if (SG(request_info).post_data==NULL) {
		return;
	}	

	var = php_strtok_r(SG(request_info).post_data, "&", &strtok_buf);

	while (var) {
		val = strchr(var, '=');
		if (val) { /* have a value */
			int val_len;

			*val++ = '\0';
			php_url_decode(var, strlen(var));
			val_len = php_url_decode(val, strlen(val));
			val_len = sapi_module.input_filter(PARSE_POST, var, &val, val_len TSRMLS_CC);
			php_register_variable_safe(var, val, val_len, array_ptr TSRMLS_CC);
		}
		var = php_strtok_r(NULL, "&", &strtok_buf);
	}
}

SAPI_API SAPI_INPUT_FILTER_FUNC(php_default_input_filter)
{
	/* TODO: check .ini setting here and apply user-defined input filter */
	return val_len;
}

SAPI_API SAPI_TREAT_DATA_FUNC(php_default_treat_data)
{
	char *res = NULL, *var, *val, *separator=NULL;
	const char *c_var;
	pval *array_ptr;
	int free_buffer=0;
	char *strtok_buf = NULL;
	
	switch (arg) {
		case PARSE_POST:
		case PARSE_GET:
		case PARSE_COOKIE:
			ALLOC_ZVAL(array_ptr);
			array_init(array_ptr);
			INIT_PZVAL(array_ptr);
			switch (arg) {
				case PARSE_POST:
					PG(http_globals)[TRACK_VARS_POST] = array_ptr;
					break;
				case PARSE_GET:
					PG(http_globals)[TRACK_VARS_GET] = array_ptr;
					break;
				case PARSE_COOKIE:
					PG(http_globals)[TRACK_VARS_COOKIE] = array_ptr;
					break;
			}
			break;
		default:
			array_ptr=destArray;
			break;
	}

	if (arg==PARSE_POST) {
		sapi_handle_post(array_ptr TSRMLS_CC);
		return;
	}

	if (arg == PARSE_GET) {		/* GET data */
		c_var = SG(request_info).query_string;
		if (c_var && *c_var) {
			res = (char *) estrdup(c_var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_COOKIE) {		/* Cookie data */
		c_var = SG(request_info).cookie_data;
		if (c_var && *c_var) {
			res = (char *) estrdup(c_var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_STRING) {		/* String data */
		res = str;
		free_buffer = 1;
	}

	if (!res) {
		return;
	}

	switch (arg) {
		case PARSE_GET:
		case PARSE_STRING:
			separator = (char *) estrdup(PG(arg_separator).input);
			break;
		case PARSE_COOKIE:
			separator = ";\0";
			break;
	}
	
	var = php_strtok_r(res, separator, &strtok_buf);
	
	while (var) {
		val = strchr(var, '=');
		if (val) { /* have a value */
			int val_len;

			*val++ = '\0';
			php_url_decode(var, strlen(var));
			val_len = php_url_decode(val, strlen(val));
			val_len = sapi_module.input_filter(arg, var, &val, val_len TSRMLS_CC);
			php_register_variable_safe(var, val, val_len, array_ptr TSRMLS_CC);
		} else {
			php_url_decode(var, strlen(var));
			php_register_variable_safe(var, "", 0, array_ptr TSRMLS_CC);
		}
		var = php_strtok_r(NULL, separator, &strtok_buf);
	}

	if(arg != PARSE_COOKIE) {
		efree(separator);
	}

	if (free_buffer) {
		efree(res);
	}
}

void _php_import_environment_variables(zval *array_ptr TSRMLS_DC)
{
	char **env, *p, *t;

	for (env = environ; env != NULL && *env != NULL; env++) {
		p = strchr(*env, '=');
		if (!p) {				/* malformed entry? */
			continue;
		}
		t = estrndup(*env, p - *env);
		php_register_variable(t, p+1, array_ptr TSRMLS_CC);
		efree(t);
	}
}


zend_bool php_std_auto_global_callback(char *name, uint name_len TSRMLS_DC)
{
	zend_printf("%s\n", name);
	return 0; /* don't rearm */
}

/* {{{ php_build_argv
 */
static void php_build_argv(char *s, zval *track_vars_array TSRMLS_DC)
{
	pval *arr, *argc, *tmp;
	int count = 0;
	char *ss, *space;
	
	ALLOC_ZVAL(arr);
	array_init(arr);
	INIT_PZVAL(arr);

	/* Prepare argv */
	if (SG(request_info).argc) { /* are we in cli sapi? */
		int i;
		for (i=0; i<SG(request_info).argc; i++) {
			ALLOC_ZVAL(tmp);
			Z_TYPE_P(tmp) = IS_STRING;
			Z_STRLEN_P(tmp) = strlen(SG(request_info).argv[i]);
			Z_STRVAL_P(tmp) = estrndup(SG(request_info).argv[i], Z_STRLEN_P(tmp));
			INIT_PZVAL(tmp);
			if (zend_hash_next_index_insert(Z_ARRVAL_P(arr), &tmp, sizeof(pval *), NULL)==FAILURE) {
				if (Z_TYPE_P(tmp) == IS_STRING) {
					efree(Z_STRVAL_P(tmp));
				}
			}
		}
	} else 	if (s && *s) {
		ss = s;
		while (ss) {
			space = strchr(ss, '+');
			if (space) {
				*space = '\0';
			}
			/* auto-type */
			ALLOC_ZVAL(tmp);
			Z_TYPE_P(tmp) = IS_STRING;
			Z_STRLEN_P(tmp) = strlen(ss);
			Z_STRVAL_P(tmp) = estrndup(ss, Z_STRLEN_P(tmp));
			INIT_PZVAL(tmp);
			count++;
			if (zend_hash_next_index_insert(Z_ARRVAL_P(arr), &tmp, sizeof(pval *), NULL)==FAILURE) {
				if (Z_TYPE_P(tmp) == IS_STRING) {
					efree(Z_STRVAL_P(tmp));
				}
			}
			if (space) {
				*space = '+';
				ss = space + 1;
			} else {
				ss = space;
			}
		}
	}

	/* prepare argc */
	ALLOC_ZVAL(argc);
	if (SG(request_info).argc) {
		Z_LVAL_P(argc) = SG(request_info).argc;
	} else {
		Z_LVAL_P(argc) = count;
	}
	Z_TYPE_P(argc) = IS_LONG;
	INIT_PZVAL(argc);

	if (PG(register_globals) || SG(request_info).argc) {
		arr->refcount++;
		argc->refcount++;
		zend_hash_update(&EG(symbol_table), "argv", sizeof("argv"), &arr, sizeof(zval *), NULL);
		zend_hash_add(&EG(symbol_table), "argc", sizeof("argc"), &argc, sizeof(zval *), NULL);
	}

	zend_hash_update(Z_ARRVAL_P(track_vars_array), "argv", sizeof("argv"), &arr, sizeof(pval *), NULL);
	zend_hash_update(Z_ARRVAL_P(track_vars_array), "argc", sizeof("argc"), &argc, sizeof(pval *), NULL);
}
/* }}} */

/* {{{ php_handle_special_queries
 */
PHPAPI int php_handle_special_queries(TSRMLS_D)
{
	if (SG(request_info).query_string && SG(request_info).query_string[0]=='=' 
			&& PG(expose_php)) {
		if (php_info_logos(SG(request_info).query_string+1 TSRMLS_CC)) {
			return 1;
		} else if (!strcmp(SG(request_info).query_string+1, PHP_CREDITS_GUID)) {
			php_print_credits(PHP_CREDITS_ALL);
			return 1;
		}
	}
	return 0;
}
/* }}} */



/* {{{ php_register_server_variables
 */
static inline void php_register_server_variables(TSRMLS_D)
{
	zval *array_ptr=NULL;

	ALLOC_ZVAL(array_ptr);
	array_init(array_ptr);
	INIT_PZVAL(array_ptr);
	PG(http_globals)[TRACK_VARS_SERVER] = array_ptr;

	/* Server variables */
	if (sapi_module.register_server_variables) {
		sapi_module.register_server_variables(array_ptr TSRMLS_CC);
	}

	/* argv/argc support */
	if (PG(register_argc_argv)) {
		php_build_argv(SG(request_info).query_string, array_ptr TSRMLS_CC);
	}

	/* PHP Authentication support */
	if (SG(request_info).auth_user) {
		php_register_variable("PHP_AUTH_USER", SG(request_info).auth_user, array_ptr TSRMLS_CC);
	}
	if (SG(request_info).auth_password) {
		php_register_variable("PHP_AUTH_PW", SG(request_info).auth_password, array_ptr TSRMLS_CC);
	}
}
/* }}} */


static zend_bool php_auto_globals_create_server(char *name, uint name_len TSRMLS_DC);
static zend_bool php_auto_globals_create_env(char *name, uint name_len TSRMLS_DC);
static zend_bool php_auto_globals_create_request(char *name, uint name_len TSRMLS_DC);

/* {{{ php_hash_environment
 */
int php_hash_environment(TSRMLS_D)
{
	char *p;
	unsigned char _gpc_flags[3] = {0, 0, 0};
	zend_bool have_variables_order;
	zval *dummy_track_vars_array = NULL;
	zend_bool initialized_dummy_track_vars_array=0;
	zend_bool jit_initialization = (!PG(register_globals) && !PG(register_long_arrays));
	char *variables_order;
	struct auto_global_record {
		char *name;
		uint name_len;
		char *long_name;
		uint long_name_len;
		zend_bool jit_initialization;
	} auto_global_records[] = {
		{ "_POST", sizeof("_POST"), "HTTP_POST_GLOBALS", sizeof("HTTP_POST_GLOBALS"), 0 },
		{ "_GET", sizeof("_GET"), "HTTP_GET_GLOBALS", sizeof("HTTP_GET_GLOBALS"), 0 },
		{ "_COOKIE", sizeof("_COOKIE"), "HTTP_COOKIE_GLOBALS", sizeof("HTTP_COOKIE_GLOBALS"), 0 },
		{ "_SERVER", sizeof("_SERVER"), "HTTP_SERVER_GLOBALS", sizeof("HTTP_SERVER_GLOBALS"), 1 },
		{ "_ENV", sizeof("_ENV"), "HTTP_ENV_GLOBALS", sizeof("HTTP_ENV_GLOBALS"), 1 },
		{ "_FILES", sizeof("_FILES"), "HTTP_FILES_GLOBALS", sizeof("HTTP_FILES_GLOBALS"), 0 },
	};
	size_t num_track_vars = sizeof(auto_global_records)/sizeof(struct auto_global_record);
	size_t i;

	/* jit_initialization = 0; */
	for (i=0; i<num_track_vars; i++) {
		PG(http_globals)[i] = NULL;
	}

	if (PG(variables_order)) {
		variables_order = PG(variables_order);
		have_variables_order=1;
	} else {
		variables_order = PG(gpc_order);
		have_variables_order=0;
		ALLOC_ZVAL(PG(http_globals)[TRACK_VARS_ENV]);
		array_init(PG(http_globals)[TRACK_VARS_ENV]);
		INIT_PZVAL(PG(http_globals)[TRACK_VARS_ENV]);
		php_import_environment_variables(PG(http_globals)[TRACK_VARS_ENV] TSRMLS_CC);
	}

	for (p=variables_order; p && *p; p++) {
		switch(*p) {
			case 'p':
			case 'P':
				if (!_gpc_flags[0] && !SG(headers_sent) && SG(request_info).request_method && !strcasecmp(SG(request_info).request_method, "POST")) {
					sapi_module.treat_data(PARSE_POST, NULL, NULL TSRMLS_CC);	/* POST Data */
					_gpc_flags[0]=1;
				}
				break;
			case 'c':
			case 'C':
				if (!_gpc_flags[1]) {
					sapi_module.treat_data(PARSE_COOKIE, NULL, NULL TSRMLS_CC);	/* Cookie Data */
					_gpc_flags[1]=1;
				}
				break;
			case 'g':
			case 'G':
				if (!_gpc_flags[2]) {
					sapi_module.treat_data(PARSE_GET, NULL, NULL TSRMLS_CC);	/* GET Data */
					_gpc_flags[2]=1;
				}
				break;
			case 'e':
			case 'E':
				if (!jit_initialization) {
					if (have_variables_order) {
						php_auto_globals_create_env("_ENV", sizeof("_ENV")-1 TSRMLS_CC);
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported 'e' element (environment) used in gpc_order - use variables_order instead");
					}
				}
				break;
			case 's':
			case 'S':
				if (!jit_initialization) {
					php_register_server_variables(TSRMLS_C);
				}
				break;
		}
	}

	if (!jit_initialization) {
		if (!have_variables_order) {
			php_register_server_variables(TSRMLS_C);
		}
		if (!PG(http_globals)[TRACK_VARS_ENV]) {

		}
	}

	for (i=0; i<num_track_vars; i++) {
		if (jit_initialization && auto_global_records[i].jit_initialization) {
			continue;
		}
		if (!PG(http_globals)[i]) {
			if (!initialized_dummy_track_vars_array) {
				ALLOC_ZVAL(dummy_track_vars_array);
				array_init(dummy_track_vars_array);
				INIT_PZVAL(dummy_track_vars_array);
				initialized_dummy_track_vars_array = 1;
			} else {
				dummy_track_vars_array->refcount++;
			}
			PG(http_globals)[i] = dummy_track_vars_array;
		}

		zend_hash_update(&EG(symbol_table), auto_global_records[i].name, auto_global_records[i].name_len, &PG(http_globals)[i], sizeof(zval *), NULL);
		PG(http_globals)[i]->refcount++;
		if (PG(register_long_arrays)) {
			zend_hash_update(&EG(symbol_table), auto_global_records[i].long_name, auto_global_records[i].long_name_len, &PG(http_globals)[i], sizeof(zval *), NULL);
			PG(http_globals)[i]->refcount++;
		}
	}

	/* Create _REQUEST */
	if (!jit_initialization) {
		php_auto_globals_create_request("_REQUEST", sizeof("_REQUEST")-1 TSRMLS_CC);
	}

	return SUCCESS;
}
/* }}} */


static zend_bool php_auto_globals_create_server(char *name, uint name_len TSRMLS_DC)
{
	php_register_server_variables(TSRMLS_C);

	zend_hash_update(&EG(symbol_table), name, name_len+1, &PG(http_globals)[TRACK_VARS_SERVER], sizeof(zval *), NULL);
	PG(http_globals)[TRACK_VARS_SERVER]->refcount++;

	if (PG(register_long_arrays)) {
		zend_hash_update(&EG(symbol_table), "HTTP_SERVER_GLOBALS", sizeof("HTTP_SERVER_GLOBALS"), &PG(http_globals)[TRACK_VARS_SERVER], sizeof(zval *), NULL);
		PG(http_globals)[TRACK_VARS_SERVER]->refcount++;
	}

	return 0; /* don't rearm */
}


static zend_bool php_auto_globals_create_env(char *name, uint name_len TSRMLS_DC)
{
	ALLOC_ZVAL(PG(http_globals)[TRACK_VARS_ENV]);
	array_init(PG(http_globals)[TRACK_VARS_ENV]);
	INIT_PZVAL(PG(http_globals)[TRACK_VARS_ENV]);
	php_import_environment_variables(PG(http_globals)[TRACK_VARS_ENV] TSRMLS_CC);

	zend_hash_update(&EG(symbol_table), name, name_len+1, &PG(http_globals)[TRACK_VARS_ENV], sizeof(zval *), NULL);
	PG(http_globals)[TRACK_VARS_ENV]->refcount++;

	if (PG(register_long_arrays)) {
		zend_hash_update(&EG(symbol_table), "HTTP_ENV_GLOBALS", sizeof("HTTP_ENV_GLOBALS"), &PG(http_globals)[TRACK_VARS_ENV], sizeof(zval *), NULL);
		PG(http_globals)[TRACK_VARS_ENV]->refcount++;
	}

	return 0; /* don't rearm */
}


static zend_bool php_auto_globals_create_request(char *name, uint name_len TSRMLS_DC)
{
	zval *form_variables;
	char *variables_order;
	char *p;

	if (PG(variables_order)) {
		variables_order = PG(variables_order);
	} else {
		variables_order = PG(gpc_order);
	}

	ALLOC_ZVAL(form_variables);
	array_init(form_variables);
	INIT_PZVAL(form_variables);

	for (p=variables_order; p && *p; p++) {
		switch (*p) {
			case 'g':
			case 'G':
				zend_hash_merge(Z_ARRVAL_P(form_variables), Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_GET]), (void (*)(void *pData)) zval_add_ref, NULL, sizeof(zval *), 1);
				break;
			case 'p':
			case 'P':
				zend_hash_merge(Z_ARRVAL_P(form_variables), Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_POST]), (void (*)(void *pData)) zval_add_ref, NULL, sizeof(zval *), 1);
				break;
			case 'c':
			case 'C':
				zend_hash_merge(Z_ARRVAL_P(form_variables), Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_COOKIE]), (void (*)(void *pData)) zval_add_ref, NULL, sizeof(zval *), 1);
				break;
		}
	}

	zend_hash_update(&EG(symbol_table), "_REQUEST", sizeof("_REQUEST"), &form_variables, sizeof(zval *), NULL);
	return 0;
}


void php_startup_auto_globals(TSRMLS_D)
{
	zend_bool cb = (!PG(register_globals) && !PG(register_long_arrays));

	/*cb = 0;*/
	zend_register_auto_global("_GET", sizeof("_GET")-1, NULL TSRMLS_CC);
	zend_register_auto_global("_POST", sizeof("_POST")-1, NULL TSRMLS_CC);
	zend_register_auto_global("_COOKIE", sizeof("_COOKIE")-1, NULL TSRMLS_CC);
	zend_register_auto_global("_SERVER", sizeof("_SERVER")-1, cb?php_auto_globals_create_server:NULL TSRMLS_CC);
	zend_register_auto_global("_ENV", sizeof("_ENV")-1, cb?php_auto_globals_create_env:NULL TSRMLS_CC);
	zend_register_auto_global("_REQUEST", sizeof("_REQUEST")-1, cb?php_auto_globals_create_request:NULL TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
