/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
  |          Derick Rethans <derick@php.net>                             |
  |          Pierre-A. Joye <pierre@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_filter.h"

ZEND_DECLARE_MODULE_GLOBALS(filter)

#include "filter_private.h"

typedef struct filter_list_entry {
	char  *name;
	int    id;
	void (*function)(PHP_INPUT_FILTER_PARAM_DECL);
} filter_list_entry;

/* {{{ filter_list */
filter_list_entry filter_list[] = {
	{ "int",             FILTER_VALIDATE_INT,           php_filter_int             },
	{ "boolean",         FILTER_VALIDATE_BOOLEAN,       php_filter_boolean         },
	{ "float",           FILTER_VALIDATE_FLOAT,         php_filter_float           },

	{ "validate_regexp", FILTER_VALIDATE_REGEXP,        php_filter_validate_regexp },
	{ "validate_url",    FILTER_VALIDATE_URL,           php_filter_validate_url    },
	{ "validate_email",  FILTER_VALIDATE_EMAIL,         php_filter_validate_email  },
	{ "validate_ip",     FILTER_VALIDATE_IP,            php_filter_validate_ip     },

	{ "string",          FILTER_SANITIZE_STRING,        php_filter_string          },
	{ "stripped",        FILTER_SANITIZE_STRING,        php_filter_string          },
	{ "encoded",         FILTER_SANITIZE_ENCODED,       php_filter_encoded         },
	{ "special_chars",   FILTER_SANITIZE_SPECIAL_CHARS, php_filter_special_chars   },
	{ "unsafe_raw",      FILTER_UNSAFE_RAW,    			php_filter_unsafe_raw      },
	{ "email",           FILTER_SANITIZE_EMAIL,         php_filter_email           },
	{ "url",             FILTER_SANITIZE_URL,           php_filter_url             },
	{ "number_int",      FILTER_SANITIZE_NUMBER_INT,    php_filter_number_int      },
	{ "number_float",    FILTER_SANITIZE_NUMBER_FLOAT,  php_filter_number_float    },
	{ "magic_quotes",    FILTER_SANITIZE_MAGIC_QUOTES,  php_filter_magic_quotes    },

	{ "callback",        FILTER_CALLBACK,               php_filter_callback        },
};
/* }}} */

#ifndef PARSE_ENV
#define PARSE_ENV 4
#endif

#ifndef PARSE_SERVER
#define PARSE_SERVER 5
#endif

#ifndef PARSE_SESSION
#define PARSE_SESSION 6
#endif

#ifndef PARSE_DATA
#define PARSE_DATA 7
#endif

static unsigned int php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC);

/* {{{ filter_functions[]
 */
zend_function_entry filter_functions[] = {
	PHP_FE(input_get, NULL)
	PHP_FE(input_get_args, NULL)
	PHP_FE(input_filters_list, NULL)
	PHP_FE(input_has_variable, NULL)
	PHP_FE(input_name_to_filter, NULL)
	PHP_FE(filter_data, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ filter_module_entry
 */
zend_module_entry filter_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"filter",
	filter_functions,
	PHP_MINIT(filter),
	PHP_MSHUTDOWN(filter),
	NULL,
	PHP_RSHUTDOWN(filter),
	PHP_MINFO(filter),
	"0.9.5-dev",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FILTER
ZEND_GET_MODULE(filter)
#endif

static PHP_INI_MH(UpdateDefaultFilter) /* {{{ */
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	for (i = 0; i < size; ++i) {
		if ((strcasecmp(new_value, filter_list[i].name) == 0)) {
			IF_G(default_filter) = filter_list[i].id;
			return SUCCESS;
		}
	}
	/* Fallback to "string" filter */
	IF_G(default_filter) = FILTER_DEFAULT;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI
 */
static PHP_INI_MH(OnUpdateFlags)
{
	if (!new_value) {
		IF_G(default_filter_flags) = FILTER_FLAG_NO_ENCODE_QUOTES;
	} else {
		IF_G(default_filter_flags) = atoi(new_value);
	}
	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("filter.default",   "unsafe_raw", PHP_INI_SYSTEM|PHP_INI_PERDIR, UpdateDefaultFilter, default_filter, zend_filter_globals, filter_globals)
	PHP_INI_ENTRY("filter.default_flags", NULL,     PHP_INI_SYSTEM|PHP_INI_PERDIR, OnUpdateFlags)
PHP_INI_END()
/* }}} */

static void php_filter_init_globals(zend_filter_globals *filter_globals) /* {{{ */
{
	filter_globals->post_array = NULL;
	filter_globals->get_array = NULL;
	filter_globals->cookie_array = NULL;
	filter_globals->env_array = NULL;
	filter_globals->server_array = NULL;
	filter_globals->session_array = NULL;
	filter_globals->default_filter = FILTER_DEFAULT;
}
/* }}} */

#define PARSE_REQUEST 99

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(filter)
{
	ZEND_INIT_MODULE_GLOBALS(filter, php_filter_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("INPUT_POST",	PARSE_POST, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_GET",		PARSE_GET, 		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_COOKIE",	PARSE_COOKIE, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_ENV",		PARSE_ENV, 		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_SERVER",	PARSE_SERVER, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_SESSION", PARSE_SESSION, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_DATA", 	PARSE_DATA, 	CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_NONE", FILTER_FLAG_NONE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_SCALAR", FILTER_FLAG_SCALAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ARRAY", FILTER_FLAG_ARRAY, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_INT", FILTER_VALIDATE_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_BOOLEAN", FILTER_VALIDATE_BOOLEAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_FLOAT", FILTER_VALIDATE_FLOAT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_REGEXP", FILTER_VALIDATE_REGEXP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_URL", FILTER_VALIDATE_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_EMAIL", FILTER_VALIDATE_EMAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_IP", FILTER_VALIDATE_IP, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_DEFAULT", FILTER_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_UNSAFE_RAW", FILTER_UNSAFE_RAW, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_STRING", FILTER_SANITIZE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_STRIPPED", FILTER_SANITIZE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_ENCODED", FILTER_SANITIZE_ENCODED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_SPECIAL_CHARS", FILTER_SANITIZE_SPECIAL_CHARS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_EMAIL", FILTER_SANITIZE_EMAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_URL", FILTER_SANITIZE_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_NUMBER_INT", FILTER_SANITIZE_NUMBER_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_NUMBER_FLOAT", FILTER_SANITIZE_NUMBER_FLOAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_MAGIC_QUOTES", FILTER_SANITIZE_MAGIC_QUOTES, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_CALLBACK", FILTER_CALLBACK, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_OCTAL", FILTER_FLAG_ALLOW_OCTAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_HEX", FILTER_FLAG_ALLOW_HEX, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_STRIP_LOW", FILTER_FLAG_STRIP_LOW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_STRIP_HIGH", FILTER_FLAG_STRIP_HIGH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ENCODE_LOW", FILTER_FLAG_ENCODE_LOW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ENCODE_HIGH", FILTER_FLAG_ENCODE_HIGH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ENCODE_AMP", FILTER_FLAG_ENCODE_AMP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_NO_ENCODE_QUOTES", FILTER_FLAG_NO_ENCODE_QUOTES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_EMPTY_STRING_NULL", FILTER_FLAG_EMPTY_STRING_NULL, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_FRACTION", FILTER_FLAG_ALLOW_FRACTION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_THOUSAND", FILTER_FLAG_ALLOW_THOUSAND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_SCIENTIFIC", FILTER_FLAG_ALLOW_SCIENTIFIC, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_SCHEME_REQUIRED", FILTER_FLAG_SCHEME_REQUIRED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_HOST_REQUIRED", FILTER_FLAG_HOST_REQUIRED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_PATH_REQUIRED", FILTER_FLAG_PATH_REQUIRED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_QUERY_REQUIRED", FILTER_FLAG_QUERY_REQUIRED, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_IPV4", FILTER_FLAG_IPV4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_IPV6", FILTER_FLAG_IPV6, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_NO_RES_RANGE", FILTER_FLAG_NO_RES_RANGE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_NO_PRIV_RANGE", FILTER_FLAG_NO_PRIV_RANGE, CONST_CS | CONST_PERSISTENT);

	sapi_register_input_filter(php_sapi_filter);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(filter)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
#define VAR_ARRAY_COPY_DTOR(a)   \
	if (IF_G(a)) {               \
		zval_ptr_dtor(&IF_G(a)); \
		IF_G(a) = NULL;          \
	}
PHP_RSHUTDOWN_FUNCTION(filter)
{
	VAR_ARRAY_COPY_DTOR(get_array)
	VAR_ARRAY_COPY_DTOR(post_array)
	VAR_ARRAY_COPY_DTOR(cookie_array)
	VAR_ARRAY_COPY_DTOR(server_array)
	VAR_ARRAY_COPY_DTOR(env_array)
	VAR_ARRAY_COPY_DTOR(session_array)
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(filter)
{
	php_info_print_table_start();
	php_info_print_table_row( 2, "Input Validation and Filtering", "enabled" );
	php_info_print_table_row( 2, "Revision", "$Revision$");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static filter_list_entry php_find_filter(long id) /* {{{ */
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	for (i = 0; i < size; ++i) {
		if (filter_list[i].id == id) {
			return filter_list[i];
		}
	}
	/* Fallback to "string" filter */
	for (i = 0; i < size; ++i) {
		if (filter_list[i].id == FILTER_DEFAULT) {
			return filter_list[i];
		}
	}
	/* To shut up GCC */
	return filter_list[0];
}
/* }}} */

static void php_zval_filter(zval *value, long filter, long flags, zval *options, char* charset TSRMLS_DC) /* {{{ */
{
	filter_list_entry  filter_func;

	filter_func = php_find_filter(filter);

	if (!filter_func.id) {
		/* Find default filter */
		filter_func = php_find_filter(FILTER_DEFAULT);
	}

	/* Here be strings */
	convert_to_string(value);

	filter_func.function(value, flags, options, charset TSRMLS_CC);
}
/* }}} */

static unsigned int php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC) /* {{{ */
{
	zval  new_var, raw_var;
	zval *array_ptr = NULL, *orig_array_ptr = NULL;
	char *orig_var;
	int retval = 0;

	assert(*val != NULL);

#define PARSE_CASE(s,a,t)                    \
		case s:                              \
			if (!IF_G(a)) {                  \
				ALLOC_ZVAL(array_ptr);       \
				array_init(array_ptr);       \
				INIT_PZVAL(array_ptr);       \
				IF_G(a) = array_ptr;         \
			} else {                         \
				array_ptr = IF_G(a);         \
			}                                \
			orig_array_ptr = PG(http_globals)[t]; \
			break;

	switch (arg) {
		PARSE_CASE(PARSE_POST,    post_array,    TRACK_VARS_POST)
		PARSE_CASE(PARSE_GET,     get_array,     TRACK_VARS_GET)
		PARSE_CASE(PARSE_COOKIE,  cookie_array,  TRACK_VARS_COOKIE)
		PARSE_CASE(PARSE_SERVER,  server_array,  TRACK_VARS_SERVER)
		PARSE_CASE(PARSE_ENV,     env_array,     TRACK_VARS_ENV)

		case PARSE_STRING: /* PARSE_STRING is used by parse_str() function */
			retval = 1;
			break;
	}

	if (array_ptr) {
		/* Make a copy of the variable name, as php_register_variable_ex seems to
		 * modify it */
		orig_var = estrdup(var);

		/* Store the RAW variable internally */
		/* FIXME: Should not use php_register_variable_ex as that also registers
		 * globals when register_globals is turned on */
		Z_STRLEN(raw_var) = val_len;
		Z_STRVAL(raw_var) = estrndup(*val, val_len + 1);
		Z_TYPE(raw_var) = IS_STRING;

		php_register_variable_ex(var, &raw_var, array_ptr TSRMLS_CC);
	}

	/* Register mangled variable */
	/* FIXME: Should not use php_register_variable_ex as that also registers
	 * globals when register_globals is turned on */
	Z_STRLEN(new_var) = val_len;
	Z_STRVAL(new_var) = estrndup(*val, val_len + 1);
	Z_TYPE(new_var) = IS_STRING;

	if (val_len) {
		if (! (IF_G(default_filter) == FILTER_UNSAFE_RAW)) {
			php_zval_filter(&new_var, IF_G(default_filter), IF_G(default_filter_flags), NULL, NULL/*charset*/ TSRMLS_CC);
		}
	}

	if (orig_array_ptr) {
		php_register_variable_ex(orig_var, &new_var, orig_array_ptr TSRMLS_CC);
	}
	if (array_ptr) {
		efree(orig_var);
	}

	if (retval) {
		if (new_val_len) {
			*new_val_len = Z_STRLEN(new_var);
		}
		efree(*val);
		if (Z_STRLEN(new_var)) {
			*val = estrndup(Z_STRVAL(new_var), Z_STRLEN(new_var) + 1);
			zval_dtor(&new_var);
		} else {
			*val = estrdup("");
		}
	}

	return retval;
}
/* }}} */

static void php_zval_filter_recursive(zval *value, long filter, long flags, zval *options, char *charset TSRMLS_DC) /* {{{ */
{
	zval **element;
	HashPosition pos;

	if (Z_TYPE_P(value) == IS_ARRAY) {
		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(value), &pos);
			 zend_hash_get_current_data_ex(Z_ARRVAL_P(value), (void **) &element, &pos) == SUCCESS;
			 zend_hash_move_forward_ex(Z_ARRVAL_P(value), &pos)) {
				php_zval_filter_recursive(*element, filter, flags, options, charset TSRMLS_CC);
		}
	} else {
		php_zval_filter(value, filter, flags, options, charset TSRMLS_CC);
	}
}
/* }}} */

static zval * php_filter_get_storage(long arg TSRMLS_DC) /* {{{ */
{
	zval * array_ptr = NULL;
	zend_bool jit_initialization = (PG(auto_globals_jit) && !PG(register_globals) && !PG(register_long_arrays));
	switch (arg) {
		case PARSE_GET:
			array_ptr = IF_G(get_array);
			break;
		case PARSE_POST:
			array_ptr = IF_G(post_array);
			break;
		case PARSE_COOKIE:
			array_ptr = IF_G(cookie_array);
			break;
		case PARSE_SERVER:
			if(jit_initialization) zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);
			array_ptr = IF_G(server_array);
			break;
		case PARSE_ENV:
			if(jit_initialization) zend_is_auto_global("_ENV", sizeof("_ENV")-1 TSRMLS_CC);
			array_ptr = IF_G(env_array);
			break;
	}

	return array_ptr;
}
/* }}} */

/* {{{ proto mixed input_has_variable(constant type, string variable_name)
 * Returns true if the variable with the name 'name' exists in source.
 */
PHP_FUNCTION(input_has_variable)
{
	long        arg;
	char       *var;
	int         var_len;
	zval      **tmp;
	zval       *array_ptr = NULL;
	HashTable  *hash_ptr;
	int         found = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &arg, &var, &var_len) == FAILURE) {
		return;
	}

	array_ptr = php_filter_get_storage(arg TSRMLS_CC);

	if (!array_ptr) {
		RETURN_FALSE;
	}

	if (!found) {
		hash_ptr = HASH_OF(array_ptr);

		if (hash_ptr && zend_hash_find(hash_ptr, var, var_len + 1, (void **)&tmp) == SUCCESS) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed input_get(constant type, string variable_name [, int filter [, mixed flags [, string charset]]])
 * Returns the filtered variable 'name'* from source `type`.
 */
PHP_FUNCTION(input_get)
{
	long        arg, filter = FILTER_DEFAULT;
	char       *var, *charset = NULL;
	int         var_len, charset_len;
	zval       *flags = NULL;
	zval      **tmp;
	zval       *array_ptr = NULL, *array_ptr2 = NULL, *array_ptr3 = NULL;
	HashTable  *hash_ptr;
	int         found = 0;
	long         filter_flags = 0;
	zval       *options = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|lzs", &arg, &var, &var_len, &filter, &flags, &charset, &charset_len) == FAILURE) {
		return;
	}

	if (flags) {
		switch (Z_TYPE_P(flags)) {
			case IS_ARRAY:
				options = flags;
				break;

			case IS_STRING:
			case IS_BOOL:
			case IS_LONG:
				convert_to_long(flags);
				filter_flags = Z_LVAL_P(flags);
				options = NULL;
				break;
		}
	}

	switch(arg) {
		case PARSE_GET:
		case PARSE_POST:
		case PARSE_COOKIE:
		case PARSE_SERVER:
		case PARSE_ENV:
			array_ptr = php_filter_get_storage(arg TSRMLS_CC);
			break;

		case PARSE_SESSION:
			/* FIXME: Implement session source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_SESSION not implemented");
			break;

		case PARSE_REQUEST:
			/* FIXME: Implement request source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_REQUEST not implemented");
			return;
			if (PG(variables_order)) {
				zval **a_ptr = &array_ptr;
				char *p, *variables_order = PG(variables_order);

				for (p = variables_order; p && *p; p++) {
					switch (*p) {
						case 'p':
						case 'P':
							if (IF_G(default_filter) != FILTER_UNSAFE_RAW) {
								*a_ptr = IF_G(post_array);
							} else {
								*a_ptr = PG(http_globals)[TRACK_VARS_POST];
							}
							break;
						case 'g':
						case 'G':
							if (IF_G(default_filter) != FILTER_UNSAFE_RAW) {
								*a_ptr = IF_G(get_array);
							} else {
								*a_ptr = PG(http_globals)[TRACK_VARS_GET];
							}
							break;
						case 'c':
						case 'C':
							if (IF_G(default_filter) != FILTER_UNSAFE_RAW) {
								*a_ptr = IF_G(cookie_array);
							} else {
								*a_ptr = PG(http_globals)[TRACK_VARS_COOKIE];
							}
							break;
					}
					if (array_ptr && !array_ptr2) {
						a_ptr = &array_ptr2;
						continue;
					}
					if (array_ptr2 && !array_ptr3) {
						a_ptr = &array_ptr3;
					}
				}
			} else {
				array_ptr = php_filter_get_storage(PARSE_GET TSRMLS_CC);
			}
	}

	if (!array_ptr) {
		RETURN_FALSE;
	}

	if (array_ptr3) {
		hash_ptr = HASH_OF(array_ptr3);
		if (hash_ptr && zend_hash_find(hash_ptr, var, var_len + 1, (void **)&tmp) == SUCCESS) {
			*return_value = **tmp;
			found = 1;
		}
	}

	if (array_ptr2 && !found) {
		hash_ptr = HASH_OF(array_ptr2);
		if (hash_ptr && zend_hash_find(hash_ptr, var, var_len + 1, (void **)&tmp) == SUCCESS) {
			*return_value = **tmp;
			found = 1;
		}
	}

	if (!found) {
		hash_ptr = HASH_OF(array_ptr);

		if (hash_ptr && zend_hash_find(hash_ptr, var, var_len + 1, (void **)&tmp) == SUCCESS) {
			*return_value = **tmp;
			found = 1;
		}
	}

	if (found) {
		zval_copy_ctor(return_value);  /* Watch out for empty strings */
		php_zval_filter_recursive(return_value, filter, filter_flags, options, charset TSRMLS_CC);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto mixed input_get_args(array definition, constant type [, array data])
 * Returns an array with all arguments defined in 'definition'.
 */
PHP_FUNCTION(input_get_args)
{
	long       filter = FILTER_DEFAULT;
	char       *charset = NULL;
	zval      **tmp, ** option;
	int         filter_flags = FILTER_FLAG_SCALAR;
	zval       *options = NULL, *temparray = NULL;

	zval *args_array, *values = NULL;
	HashTable *args_hash;
	HashPosition pos;

	HashTable * g_hash;

	long args_from = 0;
	long elm_count;
	char *key;
	unsigned int key_len;
	unsigned long index;

	/* pointers to the zval array GET, POST,... */
	zval       *array_ptr = NULL;
	zval **element;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "al|a", &args_array, &args_from, &values) == FAILURE) {
		RETURN_FALSE;
	}

	args_hash = HASH_OF(args_array);
	elm_count = zend_hash_num_elements(args_hash);

	if (elm_count < 1) {
		RETURN_NULL();
	}

	switch (args_from) {
		case PARSE_GET:
		case PARSE_POST:
		case PARSE_COOKIE:
		case PARSE_SERVER:
		case PARSE_ENV:
			array_ptr = php_filter_get_storage(args_from TSRMLS_CC);
			break;

		case PARSE_DATA:
			array_ptr = values;
			break;

		case PARSE_SESSION:
			/* FIXME: Implement session source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_SESSION not implemented");
			break;

		case PARSE_REQUEST:
			/* FIXME: Implement session source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_REQUEST not implemented");
			return;
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown INPUT method");
			return;
			break;
	}

	if (!array_ptr) {
		RETURN_FALSE;
	} else {
		g_hash = HASH_OF(array_ptr);
		zend_hash_internal_pointer_reset_ex(g_hash, &pos);
		array_init(return_value);
	}

	for (zend_hash_internal_pointer_reset_ex(args_hash, &pos);
			zend_hash_get_current_data_ex(args_hash, (void **) &element, &pos) == SUCCESS;
			zend_hash_move_forward_ex(args_hash, &pos)) {

		if (zend_hash_get_current_key_ex(args_hash, &key, &key_len, &index, 0, &pos) != HASH_KEY_IS_STRING) {
			zval_dtor(return_value);
			RETURN_FALSE;
		}

		if (g_hash && zend_hash_find(g_hash, key, key_len, (void **)&tmp) == SUCCESS) {
			if (Z_TYPE_PP(element) != IS_ARRAY) {
				convert_to_long(*element);
				filter = Z_LVAL_PP(element);
				filter_flags = FILTER_FLAG_SCALAR;

				if ((filter_flags & FILTER_FLAG_SCALAR) && Z_TYPE_PP(tmp) == IS_ARRAY) {
					/* asked for scalar and found an array do not test further */
					add_assoc_bool(return_value, key, 0);
					continue;
				}

			} else {
				if (zend_hash_find(HASH_OF(*element), "filter", sizeof("filter"), (void **)&option) == SUCCESS) {
					convert_to_long(*option);
					filter = Z_LVAL_PP(option);
				}

				if (zend_hash_find(HASH_OF(*element), "options", sizeof("options"), (void **)&option) == SUCCESS) {
					if (Z_TYPE_PP(option) == IS_ARRAY) {
						options = *option;
					}
				} else {
					options = NULL;
				}

				if (zend_hash_find(HASH_OF(*element), "flags", sizeof("flags"), (void **)&option) == SUCCESS) {

					switch (Z_TYPE_PP(option)) {
						case IS_ARRAY:
							break;
						default:
							convert_to_long(*option);
							filter_flags = Z_LVAL_PP(option);
							break;
					}
				} else {
					filter_flags = FILTER_FLAG_SCALAR;
				}

				if ((filter_flags & FILTER_FLAG_SCALAR) && Z_TYPE_PP(tmp) == IS_ARRAY) {
					/* asked for scalar and found an array do not test further */
					add_assoc_bool(return_value, key, 0);
					continue;
				}

				if (zend_hash_find(HASH_OF(*element), "charset", sizeof("charset"), (void **)&option) == SUCCESS) {
					convert_to_string(*option);
					charset = Z_STRVAL_PP(option);
				}
			}

			if (filter_flags & FILTER_FLAG_ARRAY) {
				php_zval_filter_recursive(*tmp, filter, filter_flags, options, charset TSRMLS_CC);

				/* ARRAY always returns an array */
				if (Z_TYPE_PP(tmp) != IS_ARRAY) {
					ALLOC_INIT_ZVAL(temparray);
					array_init(temparray);
					add_next_index_zval(temparray, *tmp);
					*tmp = temparray;
				}
			} else {
				php_zval_filter(*tmp, filter, filter_flags, options, charset TSRMLS_CC);
			}

			add_assoc_zval(return_value, key, *tmp);
		} else {
			add_assoc_null(return_value, key);
		}
		filter = FILTER_DEFAULT;
		filter_flags = FILTER_FLAG_SCALAR;
	}
}
/* }}} */

/* {{{ proto input_filters_list()
 * Returns a list of all supported filters */
PHP_FUNCTION(input_filters_list)
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	array_init(return_value);
	for (i = 0; i < size; ++i) {
		add_next_index_string(return_value, filter_list[i].name, 1);
	}
}
/* }}} */

/* {{{ proto input_name_to_filter(string filtername)
 * Returns the filter ID belonging to a named filter */
PHP_FUNCTION(input_name_to_filter)
{
	int i, filter_len;
	int size = sizeof(filter_list) / sizeof(filter_list_entry);
	char *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filter, &filter_len) == FAILURE) {
		return;
	}

	for (i = 0; i < size; ++i) {
		if (strcmp(filter_list[i].name, filter) == 0) {
			RETURN_LONG(filter_list[i].id);
		}
	}
	RETURN_NULL();
}
/* }}} */

/* {{{ proto filter_data(mixed variable, int filter [, mixed filter_options [, string charset ]])
 * Returns the filterd variable (scalar or array can be used).
 */
PHP_FUNCTION(filter_data)
{
	long        filter = FILTER_DEFAULT;
	char       *charset = NULL;
	int         charset_len;
	zval       *var, *flags = NULL;
	int         filter_flags = 0;
	zval       *options = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/l|zs", &var, &filter, &flags, &charset, &charset_len) == FAILURE) {
		return;
	}

	if (filter != FILTER_CALLBACK) {
		if (flags) {
			switch (Z_TYPE_P(flags)) {
				case IS_ARRAY:
					options = flags;
					break;

				case IS_STRING:
				case IS_BOOL:
				case IS_LONG:
					convert_to_long(flags);
					filter_flags = Z_LVAL_P(flags);
					options = NULL;
					break;
			}
		}
	} else {
		if (flags) {
			switch (Z_TYPE_P(flags)) {
				case IS_ARRAY:
				case IS_STRING:
					options = flags;
					break;

				default:
					convert_to_string(flags);
					options = flags;
					break;
			}
		}
	}
	php_zval_filter_recursive(var, filter, filter_flags, options, charset TSRMLS_CC);
	RETURN_ZVAL(var, 1, 0);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
