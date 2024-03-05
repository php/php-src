/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php_filter.h"

ZEND_DECLARE_MODULE_GLOBALS(filter)

#include "filter_private.h"
#include "filter_arginfo.h"

typedef struct filter_list_entry {
	const char *name;
	int    id;
	void (*function)(PHP_INPUT_FILTER_PARAM_DECL);
} filter_list_entry;

/* {{{ filter_list */
static const filter_list_entry filter_list[] = {
	{ "int",             FILTER_VALIDATE_INT,           php_filter_int             },
	{ "boolean",         FILTER_VALIDATE_BOOL,          php_filter_boolean         },
	{ "float",           FILTER_VALIDATE_FLOAT,         php_filter_float           },

	{ "validate_regexp", FILTER_VALIDATE_REGEXP,        php_filter_validate_regexp },
	{ "validate_domain", FILTER_VALIDATE_DOMAIN,        php_filter_validate_domain },
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
	{ "add_slashes",     FILTER_SANITIZE_ADD_SLASHES,   php_filter_add_slashes     },

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

static unsigned int php_sapi_filter(int arg, const char *var, char **val, size_t val_len, size_t *new_val_len);
static unsigned int php_sapi_filter_init(void);

/* {{{ filter_module_entry */
zend_module_entry filter_module_entry = {
	STANDARD_MODULE_HEADER,
	"filter",
	ext_functions,
	PHP_MINIT(filter),
	PHP_MSHUTDOWN(filter),
	NULL,
	PHP_RSHUTDOWN(filter),
	PHP_MINFO(filter),
	PHP_FILTER_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FILTER
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(filter)
#endif

static PHP_INI_MH(UpdateDefaultFilter) /* {{{ */
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	for (i = 0; i < size; ++i) {
		if ((strcasecmp(ZSTR_VAL(new_value), filter_list[i].name) == 0)) {
			IF_G(default_filter) = filter_list[i].id;
			if (IF_G(default_filter) != FILTER_DEFAULT) {
				zend_error(E_DEPRECATED, "The filter.default ini setting is deprecated");
			}
			return SUCCESS;
		}
	}
	/* Fallback to the default filter */
	IF_G(default_filter) = FILTER_DEFAULT;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI */
static PHP_INI_MH(OnUpdateFlags)
{
	if (!new_value) {
		IF_G(default_filter_flags) = FILTER_FLAG_NO_ENCODE_QUOTES;
	} else {
		IF_G(default_filter_flags) = atoi(ZSTR_VAL(new_value));
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
#if defined(COMPILE_DL_FILTER) && defined(ZTS)
ZEND_TSRMLS_CACHE_UPDATE();
#endif
	ZVAL_UNDEF(&filter_globals->post_array);
	ZVAL_UNDEF(&filter_globals->get_array);
	ZVAL_UNDEF(&filter_globals->cookie_array);
	ZVAL_UNDEF(&filter_globals->env_array);
	ZVAL_UNDEF(&filter_globals->server_array);
#if 0
	ZVAL_UNDEF(&filter_globals->session_array);
#endif
	filter_globals->default_filter = FILTER_DEFAULT;
}
/* }}} */

#define PARSE_REQUEST 99

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(filter)
{
	ZEND_INIT_MODULE_GLOBALS(filter, php_filter_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	register_filter_symbols(module_number);

	sapi_register_input_filter(php_sapi_filter, php_sapi_filter_init);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(filter)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
#define VAR_ARRAY_COPY_DTOR(a)   \
	if (!Z_ISUNDEF(IF_G(a))) {   \
		zval_ptr_dtor(&IF_G(a)); \
		ZVAL_UNDEF(&IF_G(a));    \
	}

PHP_RSHUTDOWN_FUNCTION(filter)
{
	VAR_ARRAY_COPY_DTOR(get_array)
	VAR_ARRAY_COPY_DTOR(post_array)
	VAR_ARRAY_COPY_DTOR(cookie_array)
	VAR_ARRAY_COPY_DTOR(server_array)
	VAR_ARRAY_COPY_DTOR(env_array)
#if 0
	VAR_ARRAY_COPY_DTOR(session_array)
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(filter)
{
	php_info_print_table_start();
	php_info_print_table_row( 2, "Input Validation and Filtering", "enabled" );
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static filter_list_entry php_find_filter(zend_long id) /* {{{ */
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

static unsigned int php_sapi_filter_init(void)
{
	ZVAL_UNDEF(&IF_G(get_array));
	ZVAL_UNDEF(&IF_G(post_array));
	ZVAL_UNDEF(&IF_G(cookie_array));
	ZVAL_UNDEF(&IF_G(server_array));
	ZVAL_UNDEF(&IF_G(env_array));
#if 0
	ZVAL_UNDEF(&IF_G(session_array));
#endif
	return SUCCESS;
}

static void php_zval_filter(zval *value, zend_long filter, zend_long flags, zval *options, char* charset, bool copy) /* {{{ */
{
	filter_list_entry  filter_func;

	filter_func = php_find_filter(filter);

	if (!filter_func.id) {
		/* Find default filter */
		filter_func = php_find_filter(FILTER_DEFAULT);
	}

	/* #49274, fatal error with object without a toString method
	  Fails nicely instead of getting a recovarable fatal error. */
	if (Z_TYPE_P(value) == IS_OBJECT) {
		zend_class_entry *ce;

		ce = Z_OBJCE_P(value);
		if (!ce->__tostring) {
			zval_ptr_dtor(value);
			/* #67167: doesn't return null on failure for objects */
			if (flags & FILTER_NULL_ON_FAILURE) {
				ZVAL_NULL(value);
			} else {
				ZVAL_FALSE(value);
			}
			goto handle_default;
		}
	}

	/* Here be strings */
	convert_to_string(value);

	filter_func.function(value, flags, options, charset);

handle_default:
	if (options && Z_TYPE_P(options) == IS_ARRAY &&
		((flags & FILTER_NULL_ON_FAILURE && Z_TYPE_P(value) == IS_NULL) ||
		(!(flags & FILTER_NULL_ON_FAILURE) && Z_TYPE_P(value) == IS_FALSE))) {
		zval *tmp;
		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(options), "default", sizeof("default") - 1)) != NULL) {
			ZVAL_COPY(value, tmp);
		}
	}
}
/* }}} */

static unsigned int php_sapi_filter(int arg, const char *var, char **val, size_t val_len, size_t *new_val_len) /* {{{ */
{
	zval  new_var, raw_var;
	zval *array_ptr = NULL, *orig_array_ptr = NULL;
	int retval = 0;

	assert(*val != NULL);

#define PARSE_CASE(s,a,t)                     		\
		case s:                               		\
			if (Z_ISUNDEF(IF_G(a))) {         		\
				array_init(&IF_G(a)); 				\
			}										\
			array_ptr = &IF_G(a);          			\
			orig_array_ptr = &PG(http_globals)[t]; 	\
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
	if (arg == PARSE_COOKIE && orig_array_ptr &&
			zend_symtable_str_exists(Z_ARRVAL_P(orig_array_ptr), var, strlen(var))) {
		return 0;
	}

	if (array_ptr) {
		/* Store the RAW variable internally */
		ZVAL_STRINGL(&raw_var, *val, val_len);
		php_register_variable_ex(var, &raw_var, array_ptr);
	}

	if (val_len) {
		/* Register mangled variable */
		if (IF_G(default_filter) != FILTER_UNSAFE_RAW) {
			ZVAL_STRINGL(&new_var, *val, val_len);
			php_zval_filter(&new_var, IF_G(default_filter), IF_G(default_filter_flags), NULL, NULL, 0);
		} else {
			ZVAL_STRINGL(&new_var, *val, val_len);
		}
	} else { /* empty string */
		ZVAL_EMPTY_STRING(&new_var);
	}

	if (orig_array_ptr) {
		php_register_variable_ex(var, &new_var, orig_array_ptr);
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
		zval_ptr_dtor(&new_var);
	}

	return retval;
}
/* }}} */

static void php_zval_filter_recursive(zval *value, zend_long filter, zend_long flags, zval *options, char *charset, bool copy) /* {{{ */
{
	if (Z_TYPE_P(value) == IS_ARRAY) {
		zval *element;

		if (Z_IS_RECURSIVE_P(value)) {
			return;
		}
		Z_PROTECT_RECURSION_P(value);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), element) {
			ZVAL_DEREF(element);
			if (Z_TYPE_P(element) == IS_ARRAY) {
				SEPARATE_ARRAY(element);
				php_zval_filter_recursive(element, filter, flags, options, charset, copy);
			} else {
				php_zval_filter(element, filter, flags, options, charset, copy);
			}
		} ZEND_HASH_FOREACH_END();
		Z_UNPROTECT_RECURSION_P(value);
	} else {
		php_zval_filter(value, filter, flags, options, charset, copy);
	}
}
/* }}} */

static zval *php_filter_get_storage(zend_long arg)/* {{{ */

{
	zval *array_ptr = NULL;

	switch (arg) {
		case PARSE_GET:
			array_ptr = &IF_G(get_array);
			break;
		case PARSE_POST:
			array_ptr = &IF_G(post_array);
			break;
		case PARSE_COOKIE:
			array_ptr = &IF_G(cookie_array);
			break;
		case PARSE_SERVER:
			if (PG(auto_globals_jit)) {
				zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_SERVER));
			}
			array_ptr = &IF_G(server_array);
			break;
		case PARSE_ENV:
			if (PG(auto_globals_jit)) {
				zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_ENV));
			}
			array_ptr = !Z_ISUNDEF(IF_G(env_array)) ? &IF_G(env_array) : &PG(http_globals)[TRACK_VARS_ENV];
			break;
		default:
			zend_argument_value_error(1, "must be an INPUT_* constant");
			return NULL;
	}

	if (array_ptr && Z_TYPE_P(array_ptr) != IS_ARRAY) {
		/* Storage not initialized */
		return NULL;
	}

	return array_ptr;
}
/* }}} */

/* {{{ Returns true if the variable with the name 'name' exists in source. */
PHP_FUNCTION(filter_has_var)
{
	zend_long         arg;
	zend_string *var;
	zval        *array_ptr = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lS", &arg, &var) == FAILURE) {
		RETURN_THROWS();
	}

	array_ptr = php_filter_get_storage(arg);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (array_ptr && zend_hash_exists(Z_ARRVAL_P(array_ptr), var)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

static void php_filter_call(
	zval *filtered, zend_long filter, HashTable *filter_args_ht, zend_long filter_args_long,
	const int copy, zend_long filter_flags
) /* {{{ */ {
	zval *options = NULL;
	zval *option;
	char *charset = NULL;

	if (!filter_args_ht) {
		if (filter != -1) { /* handler for array apply */
			/* filter_args is the filter_flags */
			filter_flags = filter_args_long;

			if (!(filter_flags & FILTER_REQUIRE_ARRAY ||  filter_flags & FILTER_FORCE_ARRAY)) {
				filter_flags |= FILTER_REQUIRE_SCALAR;
			}
		} else {
			filter = filter_args_long;
		}
	} else {
		if ((option = zend_hash_str_find(filter_args_ht, "filter", sizeof("filter") - 1)) != NULL) {
			filter = zval_get_long(option);
		}

		if ((option = zend_hash_str_find_deref(filter_args_ht, "options", sizeof("options") - 1)) != NULL) {
			if (filter != FILTER_CALLBACK) {
				if (Z_TYPE_P(option) == IS_ARRAY) {
					options = option;
				}
			} else {
				options = option;
				filter_flags = 0;
			}
		}

		if ((option = zend_hash_str_find(filter_args_ht, "flags", sizeof("flags") - 1)) != NULL) {
			filter_flags = zval_get_long(option);

			if (!(filter_flags & FILTER_REQUIRE_ARRAY ||  filter_flags & FILTER_FORCE_ARRAY)) {
				filter_flags |= FILTER_REQUIRE_SCALAR;
			}
		}
	}

	if (Z_TYPE_P(filtered) == IS_ARRAY) {
		if (filter_flags & FILTER_REQUIRE_SCALAR) {
			zval_ptr_dtor(filtered);
			if (filter_flags & FILTER_NULL_ON_FAILURE) {
				ZVAL_NULL(filtered);
			} else {
				ZVAL_FALSE(filtered);
			}
			return;
		}
		php_zval_filter_recursive(filtered, filter, filter_flags, options, charset, copy);
		return;
	}
	if (filter_flags & FILTER_REQUIRE_ARRAY) {
		zval_ptr_dtor(filtered);
		if (filter_flags & FILTER_NULL_ON_FAILURE) {
			ZVAL_NULL(filtered);
		} else {
			ZVAL_FALSE(filtered);
		}
		return;
	}

	php_zval_filter(filtered, filter, filter_flags, options, charset, copy);
	if (filter_flags & FILTER_FORCE_ARRAY) {
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, filtered);
		array_init(filtered);
		add_next_index_zval(filtered, &tmp);
	}
}
/* }}} */

static void php_filter_array_handler(zval *input, HashTable *op_ht, zend_long op_long,
	zval *return_value, bool add_empty
) /* {{{ */ {
	zend_string *arg_key;
	zval *tmp, *arg_elm;

	if (!op_ht) {
		ZVAL_DUP(return_value, input);
		php_filter_call(return_value, -1, NULL, op_long, 0, FILTER_REQUIRE_ARRAY);
	} else {
		array_init(return_value);

		ZEND_HASH_FOREACH_STR_KEY_VAL(op_ht, arg_key, arg_elm) {
			if (arg_key == NULL) {
				zend_argument_type_error(2, "must contain only string keys");
				RETURN_THROWS();
	 		}
			if (ZSTR_LEN(arg_key) == 0) {
				zend_argument_value_error(2, "cannot contain empty keys");
				RETURN_THROWS();
			}
			if ((tmp = zend_hash_find(Z_ARRVAL_P(input), arg_key)) == NULL) {
				if (add_empty) {
					add_assoc_null_ex(return_value, ZSTR_VAL(arg_key), ZSTR_LEN(arg_key));
				}
			} else {
				zval nval;
				ZVAL_DEREF(tmp);
				ZVAL_DUP(&nval, tmp);
				php_filter_call(&nval, -1,
					Z_TYPE_P(arg_elm) == IS_ARRAY ? Z_ARRVAL_P(arg_elm) : NULL,
					Z_TYPE_P(arg_elm) == IS_ARRAY ? 0 : zval_get_long(arg_elm),
					0, FILTER_REQUIRE_SCALAR
				);
				zend_hash_update(Z_ARRVAL_P(return_value), arg_key, &nval);
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

/* {{{ Returns the filtered variable 'name'* from source `type`. */
PHP_FUNCTION(filter_input)
{
	zend_long fetch_from, filter = FILTER_DEFAULT;
	zval *input = NULL, *tmp;
	zend_string *var;
	HashTable *filter_args_ht = NULL;
	zend_long filter_args_long = 0;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_LONG(fetch_from)
		Z_PARAM_STR(var)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(filter)
		Z_PARAM_ARRAY_HT_OR_LONG(filter_args_ht, filter_args_long)
	ZEND_PARSE_PARAMETERS_END();

	if (!PHP_FILTER_ID_EXISTS(filter)) {
		php_error_docref(NULL, E_WARNING, "Unknown filter with ID " ZEND_LONG_FMT, filter);
		RETURN_FALSE;
	}

	input = php_filter_get_storage(fetch_from);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!input || (tmp = zend_hash_find(Z_ARRVAL_P(input), var)) == NULL) {
		zend_long filter_flags = 0;
		zval *option, *opt, *def;
		if (!filter_args_ht) {
			filter_flags = filter_args_long;
		} else {
			if ((option = zend_hash_str_find(filter_args_ht, "flags", sizeof("flags") - 1)) != NULL) {
				filter_flags = zval_get_long(option);
			}

			if ((opt = zend_hash_str_find_deref(filter_args_ht, "options", sizeof("options") - 1)) != NULL &&
				Z_TYPE_P(opt) == IS_ARRAY &&
				(def = zend_hash_str_find_deref(Z_ARRVAL_P(opt), "default", sizeof("default") - 1)) != NULL
			) {
				ZVAL_COPY(return_value, def);
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

	ZVAL_DUP(return_value, tmp);

	php_filter_call(return_value, filter, filter_args_ht, filter_args_long, 1, FILTER_REQUIRE_SCALAR);
}
/* }}} */

/* {{{ Returns the filtered version of the variable. */
PHP_FUNCTION(filter_var)
{
	zend_long filter = FILTER_DEFAULT;
	zval *data;
	HashTable *filter_args_ht = NULL;
	zend_long filter_args_long = 0;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ZVAL(data)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(filter)
		Z_PARAM_ARRAY_HT_OR_LONG(filter_args_ht, filter_args_long)
	ZEND_PARSE_PARAMETERS_END();

	if (!PHP_FILTER_ID_EXISTS(filter)) {
		php_error_docref(NULL, E_WARNING, "Unknown filter with ID " ZEND_LONG_FMT, filter);
		RETURN_FALSE;
	}

	ZVAL_DUP(return_value, data);

	php_filter_call(return_value, filter, filter_args_ht, filter_args_long, 1, FILTER_REQUIRE_SCALAR);
}
/* }}} */

/* {{{ Returns an array with all arguments defined in 'definition'. */
PHP_FUNCTION(filter_input_array)
{
	zend_long    fetch_from;
	zval   *array_input = NULL;
	bool add_empty = 1;
	HashTable *op_ht = NULL;
	zend_long op_long = FILTER_DEFAULT;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(fetch_from)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_LONG(op_ht, op_long)
		Z_PARAM_BOOL(add_empty)
	ZEND_PARSE_PARAMETERS_END();

	if (!op_ht && !PHP_FILTER_ID_EXISTS(op_long)) {
		php_error_docref(NULL, E_WARNING, "Unknown filter with ID " ZEND_LONG_FMT, op_long);
		RETURN_FALSE;
	}

	array_input = php_filter_get_storage(fetch_from);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (!array_input) {
		zend_long filter_flags = 0;
		zval *option;
		if (op_long) {
			filter_flags = op_long;
		} else if (op_ht && (option = zend_hash_str_find(op_ht, "flags", sizeof("flags") - 1)) != NULL) {
			filter_flags = zval_get_long(option);
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

	php_filter_array_handler(array_input, op_ht, op_long, return_value, add_empty);
}
/* }}} */

/* {{{ Returns an array with all arguments defined in 'definition'. */
PHP_FUNCTION(filter_var_array)
{
	zval *array_input = NULL;
	bool add_empty = 1;
	HashTable *op_ht = NULL;
	zend_long op_long = FILTER_DEFAULT;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ARRAY(array_input)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_LONG(op_ht, op_long)
		Z_PARAM_BOOL(add_empty)
	ZEND_PARSE_PARAMETERS_END();

	if (!op_ht && !PHP_FILTER_ID_EXISTS(op_long)) {
		php_error_docref(NULL, E_WARNING, "Unknown filter with ID " ZEND_LONG_FMT, op_long);
		RETURN_FALSE;
	}

	php_filter_array_handler(array_input, op_ht, op_long, return_value, add_empty);
}
/* }}} */

/* {{{ Returns a list of all supported filters */
PHP_FUNCTION(filter_list)
{
	int i, size = sizeof(filter_list) / sizeof(filter_list_entry);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	for (i = 0; i < size; ++i) {
		add_next_index_string(return_value, (char *)filter_list[i].name);
	}
}
/* }}} */

/* {{{ Returns the filter ID belonging to a named filter */
PHP_FUNCTION(filter_id)
{
	int i;
	size_t filter_len;
	int size = sizeof(filter_list) / sizeof(filter_list_entry);
	char *filter;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &filter, &filter_len) == FAILURE) {
		RETURN_THROWS();
	}

	for (i = 0; i < size; ++i) {
		if (strcmp(filter_list[i].name, filter) == 0) {
			RETURN_LONG(filter_list[i].id);
		}
	}

	RETURN_FALSE;
}
/* }}} */
