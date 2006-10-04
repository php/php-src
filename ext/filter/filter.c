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
  |          Ilia Alshanetsky <iliaa@php.net>                            |
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
	const char *name;
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
	{ "unsafe_raw",      FILTER_UNSAFE_RAW,             php_filter_unsafe_raw      },
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

static unsigned int php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC);

/* {{{ filter_functions[]
 */
zend_function_entry filter_functions[] = {
	PHP_FE(filter_input,		NULL)
	PHP_FE(filter_var,		NULL)
	PHP_FE(filter_input_array,	NULL)
	PHP_FE(filter_var_array,	NULL)
	PHP_FE(filter_list,		NULL)
	PHP_FE(filter_has_var,		NULL)
	PHP_FE(filter_id,		NULL)
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
	REGISTER_LONG_CONSTANT("INPUT_GET",	PARSE_GET,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_COOKIE",	PARSE_COOKIE, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_ENV",	PARSE_ENV,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_SERVER",	PARSE_SERVER, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_SESSION", PARSE_SESSION, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_REQUEST", PARSE_REQUEST, 	CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_NONE", FILTER_FLAG_NONE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_REQUIRE_SCALAR", FILTER_REQUIRE_SCALAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_REQUIRE_ARRAY", FILTER_REQUIRE_ARRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FORCE_ARRAY", FILTER_FORCE_ARRAY, CONST_CS | CONST_PERSISTENT);

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
	php_info_print_table_header( 2, "Input Validation and Filtering", "enabled" );
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

static void php_zval_filter(zval **value, long filter, long flags, zval *options, char* charset, zend_bool copy TSRMLS_DC) /* {{{ */
{
	filter_list_entry  filter_func;

	filter_func = php_find_filter(filter);

	if (!filter_func.id) {
		/* Find default filter */
		filter_func = php_find_filter(FILTER_DEFAULT);
	}

	if (copy) {
		SEPARATE_ZVAL(value);
	}
	/* Here be strings */
	convert_to_string(*value);

	filter_func.function(*value, flags, options, charset TSRMLS_CC);
}
/* }}} */

static unsigned int php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC) /* {{{ */
{
	zval  new_var, raw_var;
	zval *array_ptr = NULL, *orig_array_ptr = NULL;
	char *orig_var = NULL;
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
		Z_STRVAL(raw_var) = estrndup(*val, val_len);
		Z_TYPE(raw_var) = IS_STRING;

		php_register_variable_ex(var, &raw_var, array_ptr TSRMLS_CC);
	}

	if (val_len) {
		/* Register mangled variable */
		/* FIXME: Should not use php_register_variable_ex as that also registers
		 * globals when register_globals is turned on */
		Z_STRLEN(new_var) = val_len;
		Z_TYPE(new_var) = IS_STRING;

		if (!(IF_G(default_filter) == FILTER_UNSAFE_RAW)) {
			zval *tmp_new_var = &new_var;
			Z_STRVAL(new_var) = estrndup(*val, val_len);
			php_zval_filter(&tmp_new_var, IF_G(default_filter), IF_G(default_filter_flags), NULL, NULL/*charset*/, 0 TSRMLS_CC);
		} else if (PG(magic_quotes_gpc)) {
			Z_STRVAL(new_var) = php_addslashes(*val, Z_STRLEN(new_var), &Z_STRLEN(new_var), 0 TSRMLS_CC);
		} else {
			Z_STRVAL(new_var) = estrndup(*val, val_len);
		}
	} else { /* empty string */
		ZVAL_EMPTY_STRING(&new_var);
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
			*val = estrndup(Z_STRVAL(new_var), Z_STRLEN(new_var));
		} else {
			*val = estrdup("");
		}
		zval_dtor(&new_var);
	}

	return retval;
}
/* }}} */

static void php_zval_filter_recursive(zval **value, long filter, long flags, zval *options, char *charset, zend_bool copy TSRMLS_DC) /* {{{ */
{
	if (Z_TYPE_PP(value) == IS_ARRAY) {
		zval **element;
		HashPosition pos;

		if (Z_ARRVAL_PP(value)->nApplyCount > 1) {
			return;
		}

		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(value), &pos);
			 zend_hash_get_current_data_ex(Z_ARRVAL_PP(value), (void **) &element, &pos) == SUCCESS;
			 zend_hash_move_forward_ex(Z_ARRVAL_PP(value), &pos)) {

				if (Z_TYPE_PP(element) == IS_ARRAY) {
					Z_ARRVAL_PP(element)->nApplyCount++;
					php_zval_filter_recursive(element, filter, flags, options, charset, copy TSRMLS_CC);
					Z_ARRVAL_PP(element)->nApplyCount--;
				} else {
					php_zval_filter(element, filter, flags, options, charset, copy TSRMLS_CC);
				}
		}
	} else {
		php_zval_filter(value, filter, flags, options, charset, copy TSRMLS_CC);
	}
}
/* }}} */

static zval *php_filter_get_storage(long arg TSRMLS_DC)/* {{{ */

{
	zval *array_ptr = NULL;
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
			if (jit_initialization) {
				zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);
			}
			array_ptr = IF_G(server_array);
			break;
		case PARSE_ENV:
			if (jit_initialization) {
				zend_is_auto_global("_ENV", sizeof("_ENV")-1 TSRMLS_CC);
			}
			array_ptr = IF_G(env_array);
			break;
		case PARSE_SESSION:
			/* FIXME: Implement session source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_SESSION is not yet implemented");
			break;
		case PARSE_REQUEST:
			/* FIXME: Implement request source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_REQUEST is not yet implemented");
			break;
	}

	return array_ptr;
}
/* }}} */

/* {{{ proto mixed filter_has_var(constant type, string variable_name)
 * Returns true if the variable with the name 'name' exists in source.
 */
PHP_FUNCTION(filter_has_var)
{
	long        arg;
	char       *var;
	int         var_len;
	zval      **tmp;
	zval       *array_ptr = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &arg, &var, &var_len) == FAILURE) {
		RETURN_FALSE;
	}

	array_ptr = php_filter_get_storage(arg TSRMLS_CC);

	if (array_ptr && HASH_OF(array_ptr) && zend_hash_find(HASH_OF(array_ptr), var, var_len + 1, (void **)&tmp) == SUCCESS) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

static void php_filter_call(zval **filtered, long filter, zval **filter_args, const int copy, long filter_flags TSRMLS_DC) /* {{{ */
{
	zval  *options = NULL;
	zval **option;
	char  *charset = NULL;

	if (filter_args && Z_TYPE_PP(filter_args) != IS_ARRAY) {
		convert_to_long_ex(filter_args);
		if (filter != -1) { /* handler for array apply */
			/* filter_args is the filter_flags */
			filter_flags = Z_LVAL_PP(filter_args);
		} else {
			filter = Z_LVAL_PP(filter_args);
		}
	} else if (filter_args) {
		if (zend_hash_find(HASH_OF(*filter_args), "filter", sizeof("filter"), (void **)&option) == SUCCESS) {
			convert_to_long(*option);
			filter = Z_LVAL_PP(option);
		}

		if (zend_hash_find(HASH_OF(*filter_args), "flags", sizeof("flags"), (void **)&option) == SUCCESS) {
			convert_to_long(*option);
			filter_flags = Z_LVAL_PP(option);

			if (!(filter_flags & FILTER_REQUIRE_ARRAY ||  filter_flags & FILTER_FORCE_ARRAY)) {
				filter_flags |= FILTER_REQUIRE_SCALAR;
			}
		}

		if (zend_hash_find(HASH_OF(*filter_args), "options", sizeof("options"), (void **)&option) == SUCCESS) {
			if (filter != FILTER_CALLBACK) {
				if (Z_TYPE_PP(option) == IS_ARRAY) {
					options = *option;
				}
			} else {
				options = *option;
				filter_flags = 0;
			}
		}
	}

	if (Z_TYPE_PP(filtered) == IS_ARRAY) {
		if (filter_flags & FILTER_REQUIRE_SCALAR) {
			if (copy) {
				SEPARATE_ZVAL(filtered);
			}
			zval_dtor(*filtered);
			ZVAL_FALSE(*filtered);
			return;
		}
		php_zval_filter_recursive(filtered, filter, filter_flags, options, charset, copy TSRMLS_CC);
		return;
	}
	if (filter_flags & FILTER_REQUIRE_ARRAY) {
		if (copy) {
			SEPARATE_ZVAL(filtered);
		}
		zval_dtor(*filtered);
		ZVAL_FALSE(*filtered);
		return;
	}

	php_zval_filter(filtered, filter, filter_flags, options, charset, copy TSRMLS_CC);
	if (filter_flags & FILTER_FORCE_ARRAY) {
		zval *tmp;

		ALLOC_ZVAL(tmp);
		*tmp = **filtered;
		zval_copy_ctor(tmp);
		INIT_PZVAL(tmp);

		zval_dtor(*filtered);

		array_init(*filtered);
		add_next_index_zval(*filtered, tmp);
	}
}
/* }}} */

static void php_filter_array_handler(zval *input, zval **op, zval *return_value TSRMLS_DC) /* {{{ */
{
	char *arg_key;
	uint arg_key_len;
	ulong index;
	HashPosition pos;
	zval **tmp, **arg_elm;

	if (!op) {
		SEPARATE_ZVAL(&input);
		*return_value = *input;
		php_filter_call(&return_value, FILTER_DEFAULT, NULL, 0, FILTER_REQUIRE_ARRAY TSRMLS_CC);
	} else if (Z_TYPE_PP(op) == IS_LONG) {
		SEPARATE_ZVAL(&input);
		*return_value = *input;
		php_filter_call(&return_value, Z_LVAL_PP(op), NULL, 0, FILTER_REQUIRE_ARRAY TSRMLS_CC);
	} else if (Z_TYPE_PP(op) == IS_ARRAY) {
		array_init(return_value);

		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(op));
		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(op), &pos);
			zend_hash_get_current_data_ex(Z_ARRVAL_PP(op), (void **) &arg_elm, &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_PP(op), &pos)) 
		{
			if (zend_hash_get_current_key_ex(Z_ARRVAL_PP(op), &arg_key, &arg_key_len, &index, 0, &pos) != HASH_KEY_IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Numeric keys are not allowed in the definition array.");
				zval_dtor(return_value);
				RETURN_FALSE;
	 		}
			if (zend_hash_find(Z_ARRVAL_P(input), arg_key, arg_key_len, (void **)&tmp) != SUCCESS) {
				add_assoc_null_ex(return_value, arg_key, arg_key_len);
			} else {
				zval *nval;

				ALLOC_ZVAL(nval);
				*nval = **tmp;
				zval_copy_ctor(nval);
				INIT_PZVAL(nval);

				php_filter_call(&nval, -1, arg_elm, 0, FILTER_REQUIRE_SCALAR TSRMLS_CC);
				add_assoc_zval_ex(return_value, arg_key, arg_key_len, nval);
			}
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto mixed filter_input(constant type, string variable_name [, long filter [, mixed options]])
 * Returns the filtered variable 'name'* from source `type`.
 */
PHP_FUNCTION(filter_input)
{
	long   fetch_from, filter = FILTER_DEFAULT;
	zval **filter_args = NULL, **data=NULL, **tmp;
	zval  *input = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lZ|lZ", &fetch_from, &data, &filter, &filter_args) == FAILURE) {
		RETURN_FALSE;
	}

	input = php_filter_get_storage(fetch_from TSRMLS_CC);

	if (!input || !HASH_OF(input) || zend_hash_find(HASH_OF(input), Z_STRVAL_PP(data), Z_STRLEN_PP(data) + 1, (void **)&tmp) != SUCCESS) {
		RETURN_FALSE;
	}

	*return_value = **tmp;
	zval_copy_ctor(return_value);  /* Watch out for empty strings */

	php_filter_call(&return_value, filter, filter_args, 1, FILTER_REQUIRE_SCALAR TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed filter_var(mixed variable [, long filter [, mixed options]])
 * Returns the filtered version of the vriable.
 */
PHP_FUNCTION(filter_var)
{
	long filter = FILTER_DEFAULT;
	zval **filter_args = NULL, *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/|lZ", &data, &filter, &filter_args) == FAILURE) {
		RETURN_FALSE;
	}

	*return_value = *data;
	zval_copy_ctor(data);

	php_filter_call(&return_value, filter, filter_args, 1, FILTER_REQUIRE_SCALAR TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed filter_input_array(constant type, [, mixed options]])
 * Returns an array with all arguments defined in 'definition'.
 */
PHP_FUNCTION(filter_input_array)
{
	long    fetch_from;
	zval   *array_input = NULL, **op = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|Z",  &fetch_from, &op) == FAILURE) {
		RETURN_FALSE;
	}

	array_input = php_filter_get_storage(fetch_from TSRMLS_CC);

	if (!array_input || !HASH_OF(array_input)) {
		RETURN_FALSE;
	}

	php_filter_array_handler(array_input, op, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed filter_var_array(array data, [, mixed options]])
 * Returns an array with all arguments defined in 'definition'.
 */
PHP_FUNCTION(filter_var_array)
{
	zval *array_input = NULL, **op = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|Z",  &array_input, &op) == FAILURE) {
		RETURN_FALSE;
	}

	php_filter_array_handler(array_input, op, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto filter_list()
 * Returns a list of all supported filters */
PHP_FUNCTION(filter_list)
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;		
	}

	array_init(return_value);
	for (i = 0; i < size; ++i) {
		add_next_index_string(return_value, (char *)filter_list[i].name, 1);
	}
}
/* }}} */

/* {{{ proto filter_id(string filtername)
 * Returns the filter ID belonging to a named filter */
PHP_FUNCTION(filter_id)
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

	RETURN_FALSE;
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
