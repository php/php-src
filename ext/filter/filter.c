/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
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
static const filter_list_entry filter_list[] = {
	{ "int",             FILTER_VALIDATE_INT,           php_filter_int             },
	{ "boolean",         FILTER_VALIDATE_BOOLEAN,       php_filter_boolean         },
	{ "float",           FILTER_VALIDATE_FLOAT,         php_filter_float           },

	{ "validate_regexp", FILTER_VALIDATE_REGEXP,        php_filter_validate_regexp },
	{ "validate_url",    FILTER_VALIDATE_URL,           php_filter_validate_url    },
	{ "validate_email",  FILTER_VALIDATE_EMAIL,         php_filter_validate_email  },
	{ "validate_ip",     FILTER_VALIDATE_IP,            php_filter_validate_ip     },
	{ "validate_mac",    FILTER_VALIDATE_MAC,           php_filter_validate_mac    },

	{ "string",          FILTER_SANITIZE_STRING,        php_filter_string          },
	{ "stripped",        FILTER_SANITIZE_STRING,        php_filter_string          },
	{ "encoded",         FILTER_SANITIZE_ENCODED,       php_filter_encoded         },
	{ "special_chars",   FILTER_SANITIZE_SPECIAL_CHARS, php_filter_special_chars   },
	{ "full_special_chars",   FILTER_SANITIZE_FULL_SPECIAL_CHARS, php_filter_full_special_chars   },
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
static unsigned int php_sapi_filter_init(TSRMLS_D);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_input, 0, 0, 2)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, variable_name)
	ZEND_ARG_INFO(0, filter)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_var, 0, 0, 1)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_INFO(0, filter)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_input_array, 0, 0, 1)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, definition)
	ZEND_ARG_INFO(0, add_empty)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_var_array, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, definition)
	ZEND_ARG_INFO(0, add_empty)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_filter_list, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_has_var, 0, 0, 2)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, variable_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_id, 0, 0, 1)
	ZEND_ARG_INFO(0, filtername)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ filter_functions[]
 */
static const zend_function_entry filter_functions[] = {
	PHP_FE(filter_input,		arginfo_filter_input)
	PHP_FE(filter_var,		arginfo_filter_var)
	PHP_FE(filter_input_array,	arginfo_filter_input_array)
	PHP_FE(filter_var_array,	arginfo_filter_var_array)
	PHP_FE(filter_list,		arginfo_filter_list)
	PHP_FE(filter_has_var,		arginfo_filter_has_var)
	PHP_FE(filter_id,		arginfo_filter_id)
	PHP_FE_END
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
	"0.11.0",
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
	/* Fallback to the default filter */
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
	REGISTER_LONG_CONSTANT("INPUT_GET",		PARSE_GET,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_COOKIE",	PARSE_COOKIE, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_ENV",		PARSE_ENV,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_SERVER",	PARSE_SERVER, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_SESSION", PARSE_SESSION, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_REQUEST", PARSE_REQUEST, 	CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_NONE", FILTER_FLAG_NONE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_REQUIRE_SCALAR", FILTER_REQUIRE_SCALAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_REQUIRE_ARRAY", FILTER_REQUIRE_ARRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FORCE_ARRAY", FILTER_FORCE_ARRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_NULL_ON_FAILURE", FILTER_NULL_ON_FAILURE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_INT", FILTER_VALIDATE_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_BOOLEAN", FILTER_VALIDATE_BOOLEAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_FLOAT", FILTER_VALIDATE_FLOAT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_REGEXP", FILTER_VALIDATE_REGEXP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_URL", FILTER_VALIDATE_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_EMAIL", FILTER_VALIDATE_EMAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_IP", FILTER_VALIDATE_IP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_MAC", FILTER_VALIDATE_MAC, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_DEFAULT", FILTER_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_UNSAFE_RAW", FILTER_UNSAFE_RAW, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_STRING", FILTER_SANITIZE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_STRIPPED", FILTER_SANITIZE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_ENCODED", FILTER_SANITIZE_ENCODED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_SPECIAL_CHARS", FILTER_SANITIZE_SPECIAL_CHARS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_FULL_SPECIAL_CHARS", FILTER_SANITIZE_FULL_SPECIAL_CHARS, CONST_CS | CONST_PERSISTENT);
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
	REGISTER_LONG_CONSTANT("FILTER_FLAG_STRIP_BACKTICK", FILTER_FLAG_STRIP_BACKTICK, CONST_CS | CONST_PERSISTENT);
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

	sapi_register_input_filter(php_sapi_filter, php_sapi_filter_init TSRMLS_CC);

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
	php_info_print_table_row( 2, "Revision", "$Id$");
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

static unsigned int php_sapi_filter_init(TSRMLS_D)
{
	IF_G(get_array) = NULL;
	IF_G(post_array) = NULL;
	IF_G(cookie_array) = NULL;
	IF_G(server_array) = NULL;
	IF_G(env_array) = NULL;
	IF_G(session_array) = NULL;
	return SUCCESS;
}

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

	/* #49274, fatal error with object without a toString method
	  Fails nicely instead of getting a recovarable fatal error. */
	if (Z_TYPE_PP(value) == IS_OBJECT) {
		zend_class_entry *ce;

		ce = Z_OBJCE_PP(value);
		if (!ce->__tostring) {
			ZVAL_FALSE(*value);
			return;
		}
	}

	/* Here be strings */
	convert_to_string(*value);

	filter_func.function(*value, flags, options, charset TSRMLS_CC);

	if (
		options && (Z_TYPE_P(options) == IS_ARRAY || Z_TYPE_P(options) == IS_OBJECT) &&
		((flags & FILTER_NULL_ON_FAILURE && Z_TYPE_PP(value) == IS_NULL) || 
		(!(flags & FILTER_NULL_ON_FAILURE) && Z_TYPE_PP(value) == IS_BOOL && Z_LVAL_PP(value) == 0)) &&
		zend_hash_exists(HASH_OF(options), "default", sizeof("default"))
	) {
		zval **tmp; 
		if (zend_hash_find(HASH_OF(options), "default", sizeof("default"), (void **)&tmp) == SUCCESS) {
			MAKE_COPY_ZVAL(tmp, *value);
		}
	}
}
/* }}} */

static unsigned int php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC) /* {{{ */
{
	zval  new_var, raw_var;
	zval *array_ptr = NULL, *orig_array_ptr = NULL;
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

	/* 
	 * According to rfc2965, more specific paths are listed above the less specific ones.
	 * If we encounter a duplicate cookie name, we should skip it, since it is not possible
	 * to have the same (plain text) cookie name for the same path and we should not overwrite
	 * more specific cookies with the less specific ones.
	*/
	if (arg == PARSE_COOKIE && orig_array_ptr && zend_symtable_exists(Z_ARRVAL_P(orig_array_ptr), var, strlen(var)+1)) {
		return 0;
	}

	if (array_ptr) {
		/* Store the RAW variable internally */
		Z_STRLEN(raw_var) = val_len;
		Z_STRVAL(raw_var) = estrndup(*val, val_len);
		Z_TYPE(raw_var) = IS_STRING;

		php_register_variable_ex(var, &raw_var, array_ptr TSRMLS_CC);
	}

	if (val_len) {
		/* Register mangled variable */
		Z_STRLEN(new_var) = val_len;
		Z_TYPE(new_var) = IS_STRING;

		if (IF_G(default_filter) != FILTER_UNSAFE_RAW) {
			zval *tmp_new_var = &new_var;
			Z_STRVAL(new_var) = estrndup(*val, val_len);
			INIT_PZVAL(tmp_new_var);
			php_zval_filter(&tmp_new_var, IF_G(default_filter), IF_G(default_filter_flags), NULL, NULL/*charset*/, 0 TSRMLS_CC);
		} else {
			Z_STRVAL(new_var) = estrndup(*val, val_len);
		}
	} else { /* empty string */
		ZVAL_EMPTY_STRING(&new_var);
	}

	if (orig_array_ptr) {
		php_register_variable_ex(var, &new_var, orig_array_ptr TSRMLS_CC);
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
			 zend_hash_move_forward_ex(Z_ARRVAL_PP(value), &pos)
		) {
			SEPARATE_ZVAL_IF_NOT_REF(element);
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
			if (PG(auto_globals_jit)) {
				zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);
			}
			array_ptr = IF_G(server_array);
			break;
		case PARSE_ENV:
			if (PG(auto_globals_jit)) {
				zend_is_auto_global("_ENV", sizeof("_ENV")-1 TSRMLS_CC);
			}
			array_ptr = IF_G(env_array) ? IF_G(env_array) : PG(http_globals)[TRACK_VARS_ENV];
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
	zval       *array_ptr = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &arg, &var, &var_len) == FAILURE) {
		RETURN_FALSE;
	}

	array_ptr = php_filter_get_storage(arg TSRMLS_CC);

	if (array_ptr && HASH_OF(array_ptr) && zend_hash_exists(HASH_OF(array_ptr), var, var_len + 1)) {
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
		long lval;

		PHP_FILTER_GET_LONG_OPT(filter_args, lval);

		if (filter != -1) { /* handler for array apply */
			/* filter_args is the filter_flags */
			filter_flags = lval;

			if (!(filter_flags & FILTER_REQUIRE_ARRAY ||  filter_flags & FILTER_FORCE_ARRAY)) {
				filter_flags |= FILTER_REQUIRE_SCALAR;
			}
		} else {
			filter = lval;
		}
	} else if (filter_args) {
		if (zend_hash_find(HASH_OF(*filter_args), "filter", sizeof("filter"), (void **)&option) == SUCCESS) {
			PHP_FILTER_GET_LONG_OPT(option, filter);
		}

		if (zend_hash_find(HASH_OF(*filter_args), "flags", sizeof("flags"), (void **)&option) == SUCCESS) {
			PHP_FILTER_GET_LONG_OPT(option, filter_flags);

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
			if (filter_flags & FILTER_NULL_ON_FAILURE) {
				ZVAL_NULL(*filtered);
			} else {
				ZVAL_FALSE(*filtered);
			}
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
		if (filter_flags & FILTER_NULL_ON_FAILURE) {
			ZVAL_NULL(*filtered);
		} else {
			ZVAL_FALSE(*filtered);
		}
		return;
	}

	php_zval_filter(filtered, filter, filter_flags, options, charset, copy TSRMLS_CC);
	if (filter_flags & FILTER_FORCE_ARRAY) {
		zval *tmp;

		ALLOC_ZVAL(tmp);
		MAKE_COPY_ZVAL(filtered, tmp);

		zval_dtor(*filtered);

		array_init(*filtered);
		add_next_index_zval(*filtered, tmp);
	}
}
/* }}} */

static void php_filter_array_handler(zval *input, zval **op, zval *return_value, zend_bool add_empty TSRMLS_DC) /* {{{ */
{
	char *arg_key;
	uint arg_key_len;
	ulong index;
	HashPosition pos;
	zval **tmp, **arg_elm;

	if (!op) {
		zval_dtor(return_value);
		MAKE_COPY_ZVAL(&input, return_value);
		php_filter_call(&return_value, FILTER_DEFAULT, NULL, 0, FILTER_REQUIRE_ARRAY TSRMLS_CC);
	} else if (Z_TYPE_PP(op) == IS_LONG) {
		zval_dtor(return_value);
		MAKE_COPY_ZVAL(&input, return_value);
		php_filter_call(&return_value, Z_LVAL_PP(op), NULL, 0, FILTER_REQUIRE_ARRAY TSRMLS_CC);
	} else if (Z_TYPE_PP(op) == IS_ARRAY) {
		array_init(return_value);

		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(op));
		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(op), &pos);
			zend_hash_get_current_data_ex(Z_ARRVAL_PP(op), (void **) &arg_elm, &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_PP(op), &pos))
		{
			if (zend_hash_get_current_key_ex(Z_ARRVAL_PP(op), &arg_key, &arg_key_len, &index, 0, &pos) != HASH_KEY_IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Numeric keys are not allowed in the definition array");
				zval_dtor(return_value);
				RETURN_FALSE;
	 		}
			if (arg_key_len < 2) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty keys are not allowed in the definition array");
				zval_dtor(return_value);
				RETURN_FALSE;
			}
			if (zend_hash_find(Z_ARRVAL_P(input), arg_key, arg_key_len, (void **)&tmp) != SUCCESS) {
				if (add_empty) {
					add_assoc_null_ex(return_value, arg_key, arg_key_len);
				}
			} else {
				zval *nval;

				ALLOC_ZVAL(nval);
				MAKE_COPY_ZVAL(tmp, nval);

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
	zval **filter_args = NULL, **tmp;
	zval  *input = NULL;
	char *var;
	int var_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|lZ", &fetch_from, &var, &var_len, &filter, &filter_args) == FAILURE) {
		return;
	}

	if (!PHP_FILTER_ID_EXISTS(filter)) {
		RETURN_FALSE;
	}

	input = php_filter_get_storage(fetch_from TSRMLS_CC);

	if (!input || !HASH_OF(input) || zend_hash_find(HASH_OF(input), var, var_len + 1, (void **)&tmp) != SUCCESS) {
		long filter_flags = 0;
		zval **option, **opt, **def;
		if (filter_args) {
			if (Z_TYPE_PP(filter_args) == IS_LONG) {
				filter_flags = Z_LVAL_PP(filter_args);
			} else if (Z_TYPE_PP(filter_args) == IS_ARRAY && zend_hash_find(HASH_OF(*filter_args), "flags", sizeof("flags"), (void **)&option) == SUCCESS) {
				PHP_FILTER_GET_LONG_OPT(option, filter_flags);
			}
			if (Z_TYPE_PP(filter_args) == IS_ARRAY && 
				zend_hash_find(HASH_OF(*filter_args), "options", sizeof("options"), (void **)&opt) == SUCCESS &&
				Z_TYPE_PP(opt) == IS_ARRAY &&
				zend_hash_find(HASH_OF(*opt), "default", sizeof("default"), (void **)&def) == SUCCESS
			) {
				MAKE_COPY_ZVAL(def, return_value);
				return;
			}
		}

		/* The FILTER_NULL_ON_FAILURE flag inverts the usual return values of
		 * the function: normally when validation fails false is returned, and
		 * when the input value doesn't exist NULL is returned. With the flag
		 * set, NULL and false should be returned, respectively. Ergo, although
		 * the code below looks incorrect, it's actually right. */
		if (filter_flags & FILTER_NULL_ON_FAILURE) {
			RETURN_FALSE;
		} else {
			RETURN_NULL();
		}
	}

	MAKE_COPY_ZVAL(tmp, return_value);

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
		return;
	}

	if (!PHP_FILTER_ID_EXISTS(filter)) {
		RETURN_FALSE;
	}

	MAKE_COPY_ZVAL(&data, return_value);

	php_filter_call(&return_value, filter, filter_args, 1, FILTER_REQUIRE_SCALAR TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed filter_input_array(constant type, [, mixed options [, bool add_empty]]])
 * Returns an array with all arguments defined in 'definition'.
 */
PHP_FUNCTION(filter_input_array)
{
	long    fetch_from;
	zval   *array_input = NULL, **op = NULL;
	zend_bool add_empty = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|Zb",  &fetch_from, &op, &add_empty) == FAILURE) {
		return;
	}

	if (op
		&& (Z_TYPE_PP(op) != IS_ARRAY)
		&& (Z_TYPE_PP(op) == IS_LONG && !PHP_FILTER_ID_EXISTS(Z_LVAL_PP(op)))
		) {
		RETURN_FALSE;
	}

	array_input = php_filter_get_storage(fetch_from TSRMLS_CC);

	if (!array_input || !HASH_OF(array_input)) {
		long filter_flags = 0;
		zval **option;
		if (op) {
			if (Z_TYPE_PP(op) == IS_LONG) {
				filter_flags = Z_LVAL_PP(op);
			} else if (Z_TYPE_PP(op) == IS_ARRAY && zend_hash_find(HASH_OF(*op), "flags", sizeof("flags"), (void **)&option) == SUCCESS) {
				PHP_FILTER_GET_LONG_OPT(option, filter_flags);
			}
		}

		/* The FILTER_NULL_ON_FAILURE flag inverts the usual return values of
		 * the function: normally when validation fails false is returned, and
		 * when the input value doesn't exist NULL is returned. With the flag
		 * set, NULL and false should be returned, respectively. Ergo, although
		 * the code below looks incorrect, it's actually right. */
		if (filter_flags & FILTER_NULL_ON_FAILURE) {
			RETURN_FALSE;
		} else {
			RETURN_NULL();
		}
	}

	php_filter_array_handler(array_input, op, return_value, add_empty TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed filter_var_array(array data, [, mixed options [, bool add_empty]]])
 * Returns an array with all arguments defined in 'definition'.
 */
PHP_FUNCTION(filter_var_array)
{
	zval *array_input = NULL, **op = NULL;
	zend_bool add_empty = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|Zb",  &array_input, &op, &add_empty) == FAILURE) {
		return;
	}

	if (op
		&& (Z_TYPE_PP(op) != IS_ARRAY)
		&& (Z_TYPE_PP(op) == IS_LONG && !PHP_FILTER_ID_EXISTS(Z_LVAL_PP(op)))
		) {
		RETURN_FALSE;
	}

	php_filter_array_handler(array_input, op, return_value, add_empty TSRMLS_CC);
}
/* }}} */

/* {{{ proto filter_list()
 * Returns a list of all supported filters */
PHP_FUNCTION(filter_list)
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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
