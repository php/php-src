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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
 */

#include <stdio.h>
#include "php.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/credits.h"
#include "zend_smart_str.h"
#include "php_variables.h"
#include "php_globals.h"
#include "php_content_types.h"
#include "SAPI.h"
#include "zend_globals.h"
#include "zend_exceptions.h"

/* for systems that need to override reading of environment variables */
void _php_import_environment_variables(zval *array_ptr);
void _php_load_environment_variables(zval *array_ptr);
PHPAPI void (*php_import_environment_variables)(zval *array_ptr) = _php_import_environment_variables;
PHPAPI void (*php_load_environment_variables)(zval *array_ptr) = _php_load_environment_variables;

PHPAPI void php_register_variable(const char *var, const char *strval, zval *track_vars_array)
{
	php_register_variable_safe(var, strval, strlen(strval), track_vars_array);
}

/* binary-safe version */
PHPAPI void php_register_variable_safe(const char *var, const char *strval, size_t str_len, zval *track_vars_array)
{
	zval new_entry;
	assert(strval != NULL);

	ZVAL_STRINGL_FAST(&new_entry, strval, str_len);

	php_register_variable_ex(var, &new_entry, track_vars_array);
}

static zend_always_inline void php_register_variable_quick(const char *name, size_t name_len, zval *val, HashTable *ht)
{
	zend_string *key = zend_string_init_interned(name, name_len, 0);

	zend_hash_update_ind(ht, key, val);
	zend_string_release_ex(key, 0);
}

PHPAPI void php_register_known_variable(const char *var_name, size_t var_name_len, zval *value, zval *track_vars_array)
{
	HashTable *symbol_table = NULL;

	ZEND_ASSERT(var_name != NULL);
	ZEND_ASSERT(var_name_len != 0);
	ZEND_ASSERT(track_vars_array != NULL && Z_TYPE_P(track_vars_array) == IS_ARRAY);

	symbol_table = Z_ARRVAL_P(track_vars_array);

#if ZEND_DEBUG
	/* Verify the name is valid for a PHP variable */
	ZEND_ASSERT(!(var_name_len == strlen("GLOBALS") && !memcmp(var_name, "GLOBALS", strlen("GLOBALS"))));
	ZEND_ASSERT(!(var_name_len == strlen("this") && !memcmp(var_name, "this", strlen("this"))));

	/* Assert that the variable name is not numeric */
	zend_ulong idx;
	ZEND_ASSERT(!ZEND_HANDLE_NUMERIC_STR(var_name, var_name_len, idx));
	/* ensure that we don't have null bytes, spaces, dots, or array bracket in the variable name (not binary safe) */
	const char *p = var_name;
	for (size_t l = 0; l < var_name_len; l++) {
		ZEND_ASSERT(*p != '\0' && *p != ' ' && *p != '.' && *p != '[');
		p++;
	}

	/* Do not allow to register cookies this way */
	ZEND_ASSERT(Z_TYPE(PG(http_globals)[TRACK_VARS_COOKIE]) == IS_UNDEF ||
		Z_ARRVAL(PG(http_globals)[TRACK_VARS_COOKIE]) != symbol_table);
#endif

	php_register_variable_quick(var_name, var_name_len, value, symbol_table);
}

PHPAPI void php_register_variable_ex(const char *var_name, zval *val, zval *track_vars_array)
{
	char *p = NULL;
	char *ip = NULL;		/* index pointer */
	char *index;
	char *var, *var_orig;
	size_t var_len, index_len;
	zval gpc_element, *gpc_element_p;
	bool is_array = 0;
	HashTable *symtable1 = NULL;
	ALLOCA_FLAG(use_heap)

	assert(var_name != NULL);

	if (track_vars_array && Z_TYPE_P(track_vars_array) == IS_ARRAY) {
		symtable1 = Z_ARRVAL_P(track_vars_array);
	}

	if (!symtable1) {
		/* Nothing to do */
		zval_ptr_dtor_nogc(val);
		return;
	}


	/* ignore leading spaces in the variable name */
	while (*var_name==' ') {
		var_name++;
	}

	/*
	 * Prepare variable name
	 */
	var_len = strlen(var_name);
	var = var_orig = do_alloca(var_len + 1, use_heap);
	memcpy(var_orig, var_name, var_len + 1);

	/* ensure that we don't have spaces or dots in the variable name (not binary safe) */
	for (p = var; *p; p++) {
		if (*p == ' ' || *p == '.') {
			*p='_';
		} else if (*p == '[') {
			is_array = 1;
			ip = p;
			*p = 0;
			break;
		}
	}
	var_len = p - var;

	/* Discard variable if mangling made it start with __Host-, where pre-mangling it did not start with __Host- */
	if (strncmp(var, "__Host-", sizeof("__Host-")-1) == 0 && strncmp(var_name, "__Host-", sizeof("__Host-")-1) != 0) {
		zval_ptr_dtor_nogc(val);
		free_alloca(var_orig, use_heap);
		return;
	}

	/* Discard variable if mangling made it start with __Secure-, where pre-mangling it did not start with __Secure- */
	if (strncmp(var, "__Secure-", sizeof("__Secure-")-1) == 0 && strncmp(var_name, "__Secure-", sizeof("__Secure-")-1) != 0) {
		zval_ptr_dtor_nogc(val);
		free_alloca(var_orig, use_heap);
		return;
	}

	if (var_len==0) { /* empty variable name, or variable name with a space in it */
		zval_ptr_dtor_nogc(val);
		free_alloca(var_orig, use_heap);
		return;
	}

	if (var_len == sizeof("this")-1 && EG(current_execute_data)) {
		zend_execute_data *ex = EG(current_execute_data);

		while (ex) {
			if (ex->func && ZEND_USER_CODE(ex->func->common.type)) {
				if ((ZEND_CALL_INFO(ex) & ZEND_CALL_HAS_SYMBOL_TABLE)
						&& ex->symbol_table == symtable1) {
					if (memcmp(var, "this", sizeof("this")-1) == 0) {
						zend_throw_error(NULL, "Cannot re-assign $this");
						zval_ptr_dtor_nogc(val);
						free_alloca(var_orig, use_heap);
						return;
					}
				}
				break;
			}
			ex = ex->prev_execute_data;
		}
	}

	/* GLOBALS hijack attempt, reject parameter */
	if (symtable1 == &EG(symbol_table) &&
		var_len == sizeof("GLOBALS")-1 &&
		!memcmp(var, "GLOBALS", sizeof("GLOBALS")-1)) {
		zval_ptr_dtor_nogc(val);
		free_alloca(var_orig, use_heap);
		return;
	}

	index = var;
	index_len = var_len;

	if (is_array) {
		int nest_level = 0;
		while (1) {
			char *index_s;
			size_t new_idx_len = 0;

			if(++nest_level > PG(max_input_nesting_level)) {
				HashTable *ht;
				/* too many levels of nesting */

				if (track_vars_array) {
					ht = Z_ARRVAL_P(track_vars_array);
					zend_symtable_str_del(ht, var, var_len);
				}

				zval_ptr_dtor_nogc(val);

				/* do not output the error message to the screen,
				 this helps us to avoid "information disclosure" */
				if (!PG(display_errors)) {
					php_error_docref(NULL, E_WARNING, "Input variable nesting level exceeded " ZEND_LONG_FMT ". To increase the limit change max_input_nesting_level in php.ini.", PG(max_input_nesting_level));
				}
				free_alloca(var_orig, use_heap);
				return;
			}

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
					/* not an index; un-terminate the var name */
					*(index_s - 1) = '_';
					/* PHP variables cannot contain ' ', '.', '[' in their names, so we replace the characters with a '_' */
					for (p = index_s; *p; p++) {
						if (*p == ' ' || *p == '.' || *p == '[') {
							*p = '_';
						}
					}

					index_len = 0;
					if (index) {
						index_len = strlen(index);
					}
					goto plain_var;
					return;
				}
				*ip = 0;
				new_idx_len = strlen(index_s);
			}

			if (!index) {
				array_init(&gpc_element);
				if ((gpc_element_p = zend_hash_next_index_insert(symtable1, &gpc_element)) == NULL) {
					zend_array_destroy(Z_ARR(gpc_element));
					zval_ptr_dtor_nogc(val);
					free_alloca(var_orig, use_heap);
					return;
				}
			} else {
				gpc_element_p = zend_symtable_str_find(symtable1, index, index_len);
				if (!gpc_element_p) {
					zval tmp;
					array_init(&tmp);
					gpc_element_p = zend_symtable_str_update_ind(symtable1, index, index_len, &tmp);
				} else {
					if (Z_TYPE_P(gpc_element_p) == IS_INDIRECT) {
						gpc_element_p = Z_INDIRECT_P(gpc_element_p);
					}
					if (Z_TYPE_P(gpc_element_p) != IS_ARRAY) {
						zval_ptr_dtor_nogc(gpc_element_p);
						array_init(gpc_element_p);
					} else {
						SEPARATE_ARRAY(gpc_element_p);
					}
				}
			}
			symtable1 = Z_ARRVAL_P(gpc_element_p);
			/* ip pointed to the '[' character, now obtain the key */
			index = index_s;
			index_len = new_idx_len;

			ip++;
			if (*ip == '[') {
				is_array = 1;
				*ip = 0;
			} else {
				goto plain_var;
			}
		}
	} else {
plain_var:
		if (!index) {
			if (zend_hash_next_index_insert(symtable1, val) == NULL) {
				zval_ptr_dtor_nogc(val);
			}
		} else {
			zend_ulong idx;

			/*
			 * According to rfc2965, more specific paths are listed above the less specific ones.
			 * If we encounter a duplicate cookie name, we should skip it, since it is not possible
			 * to have the same (plain text) cookie name for the same path and we should not overwrite
			 * more specific cookies with the less specific ones.
			 */
			if (Z_TYPE(PG(http_globals)[TRACK_VARS_COOKIE]) != IS_UNDEF &&
				symtable1 == Z_ARRVAL(PG(http_globals)[TRACK_VARS_COOKIE]) &&
				zend_symtable_str_exists(symtable1, index, index_len)) {
				zval_ptr_dtor_nogc(val);
			} else if (ZEND_HANDLE_NUMERIC_STR(index, index_len, idx)) {
				zend_hash_index_update(symtable1, idx, val);
			} else {
				php_register_variable_quick(index, index_len, val, symtable1);
			}
		}
	}
	free_alloca(var_orig, use_heap);
}

typedef struct post_var_data {
	smart_str str;
	char *ptr;
	char *end;
	uint64_t cnt;

	/* Bytes in ptr that have already been scanned for '&' */
	size_t already_scanned;
} post_var_data_t;

static bool add_post_var(zval *arr, post_var_data_t *var, bool eof)
{
	char *start, *ksep, *vsep, *val;
	size_t klen, vlen;
	size_t new_vlen;

	if (var->ptr >= var->end) {
		return 0;
	}

	start = var->ptr + var->already_scanned;
	vsep = memchr(start, '&', var->end - start);
	if (!vsep) {
		if (!eof) {
			var->already_scanned = var->end - var->ptr;
			return 0;
		} else {
			vsep = var->end;
		}
	}

	ksep = memchr(var->ptr, '=', vsep - var->ptr);
	if (ksep) {
		*ksep = '\0';
		/* "foo=bar&" or "foo=&" */
		klen = ksep - var->ptr;
		vlen = vsep - ++ksep;
	} else {
		ksep = "";
		/* "foo&" */
		klen = vsep - var->ptr;
		vlen = 0;
	}

	php_url_decode(var->ptr, klen);

	val = estrndup(ksep, vlen);
	if (vlen) {
		vlen = php_url_decode(val, vlen);
	}

	if (sapi_module.input_filter(PARSE_POST, var->ptr, &val, vlen, &new_vlen)) {
		php_register_variable_safe(var->ptr, val, new_vlen, arr);
	}
	efree(val);

	var->ptr = vsep + (vsep != var->end);
	var->already_scanned = 0;
	return 1;
}

static inline int add_post_vars(zval *arr, post_var_data_t *vars, bool eof)
{
	uint64_t max_vars = REQUEST_PARSE_BODY_OPTION_GET(max_input_vars, PG(max_input_vars));

	vars->ptr = ZSTR_VAL(vars->str.s);
	vars->end = ZSTR_VAL(vars->str.s) + ZSTR_LEN(vars->str.s);
	while (add_post_var(arr, vars, eof)) {
		if (++vars->cnt > max_vars) {
			php_error_docref(NULL, E_WARNING,
					"Input variables exceeded %" PRIu64 ". "
					"To increase the limit change max_input_vars in php.ini.",
					max_vars);
			return FAILURE;
		}
	}

	if (!eof && ZSTR_VAL(vars->str.s) != vars->ptr) {
		memmove(ZSTR_VAL(vars->str.s), vars->ptr, ZSTR_LEN(vars->str.s) = vars->end - vars->ptr);
	}
	return SUCCESS;
}

#ifdef PHP_WIN32
#define SAPI_POST_HANDLER_BUFSIZ 16384
#else
# define SAPI_POST_HANDLER_BUFSIZ BUFSIZ
#endif
SAPI_API SAPI_POST_HANDLER_FUNC(php_std_post_handler)
{
	zval *arr = (zval *) arg;
	php_stream *s = SG(request_info).request_body;
	post_var_data_t post_data;

	if (s && SUCCESS == php_stream_rewind(s)) {
		memset(&post_data, 0, sizeof(post_data));

		while (!php_stream_eof(s)) {
			char buf[SAPI_POST_HANDLER_BUFSIZ] = {0};
			ssize_t len = php_stream_read(s, buf, SAPI_POST_HANDLER_BUFSIZ);

			if (len > 0) {
				smart_str_appendl(&post_data.str, buf, len);

				if (SUCCESS != add_post_vars(arr, &post_data, 0)) {
					smart_str_free(&post_data.str);
					return;
				}
			}

			if (len != SAPI_POST_HANDLER_BUFSIZ){
				break;
			}
		}

		if (post_data.str.s) {
			add_post_vars(arr, &post_data, 1);
			smart_str_free(&post_data.str);
		}
	}
}
#undef SAPI_POST_HANDLER_BUFSIZ

SAPI_API SAPI_INPUT_FILTER_FUNC(php_default_input_filter)
{
	/* TODO: check .ini setting here and apply user-defined input filter */
	if(new_val_len) *new_val_len = val_len;
	return 1;
}

SAPI_API SAPI_TREAT_DATA_FUNC(php_default_treat_data)
{
	char *res = NULL, *var, *val, *separator = NULL;
	const char *c_var;
	zval array;
	int free_buffer = 0;
	char *strtok_buf = NULL;
	zend_long count = 0;

	ZVAL_UNDEF(&array);
	switch (arg) {
		case PARSE_POST:
		case PARSE_GET:
		case PARSE_COOKIE:
			array_init(&array);
			switch (arg) {
				case PARSE_POST:
					zval_ptr_dtor_nogc(&PG(http_globals)[TRACK_VARS_POST]);
					ZVAL_COPY_VALUE(&PG(http_globals)[TRACK_VARS_POST], &array);
					break;
				case PARSE_GET:
					zval_ptr_dtor_nogc(&PG(http_globals)[TRACK_VARS_GET]);
					ZVAL_COPY_VALUE(&PG(http_globals)[TRACK_VARS_GET], &array);
					break;
				case PARSE_COOKIE:
					zval_ptr_dtor_nogc(&PG(http_globals)[TRACK_VARS_COOKIE]);
					ZVAL_COPY_VALUE(&PG(http_globals)[TRACK_VARS_COOKIE], &array);
					break;
			}
			break;
		default:
			ZVAL_COPY_VALUE(&array, destArray);
			break;
	}

	if (arg == PARSE_POST) {
		sapi_handle_post(&array);
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
			separator = PG(arg_separator).input;
			break;
		case PARSE_COOKIE:
			separator = ";\0";
			break;
	}

	var = php_strtok_r(res, separator, &strtok_buf);

	while (var) {
		size_t val_len;
		size_t new_val_len;

		val = strchr(var, '=');

		if (arg == PARSE_COOKIE) {
			/* Remove leading spaces from cookie names, needed for multi-cookie header where ; can be followed by a space */
			while (isspace(*var)) {
				var++;
			}
			if (var == val || *var == '\0') {
				goto next_cookie;
			}
		}

		zend_long max_input_vars = REQUEST_PARSE_BODY_OPTION_GET(max_input_vars, PG(max_input_vars));
		if (++count > max_input_vars) {
			php_error_docref(NULL, E_WARNING, "Input variables exceeded " ZEND_LONG_FMT ". To increase the limit change max_input_vars in php.ini.", max_input_vars);
			break;
		}

		if (val) { /* have a value */

			*val++ = '\0';

			if (arg == PARSE_COOKIE) {
				val_len = php_raw_url_decode(val, strlen(val));
			} else {
				val_len = php_url_decode(val, strlen(val));
			}
		} else {
			val     = "";
			val_len =  0;
		}

		val = estrndup(val, val_len);
		if (arg != PARSE_COOKIE) {
			php_url_decode(var, strlen(var));
		}
		if (sapi_module.input_filter(arg, var, &val, val_len, &new_val_len)) {
			php_register_variable_safe(var, val, new_val_len, &array);
		}
		efree(val);
next_cookie:
		var = php_strtok_r(NULL, separator, &strtok_buf);
	}

	if (free_buffer) {
		efree(res);
	}
}

static zend_always_inline int valid_environment_name(const char *name, const char *end)
{
	const char *s;

	for (s = name; s < end; s++) {
		if (*s == ' ' || *s == '.' || *s == '[') {
			return 0;
		}
	}
	return 1;
}

static zend_always_inline void import_environment_variable(HashTable *ht, char *env)
{
	char *p;
	size_t name_len, len;
	zval val;
	zend_ulong idx;

	p = strchr(env, '=');
	if (!p
		|| p == env
		|| !valid_environment_name(env, p)) {
		/* malformed entry? */
		return;
	}
	name_len = p - env;
	p++;
	len = strlen(p);
	ZVAL_STRINGL_FAST(&val, p, len);
	if (ZEND_HANDLE_NUMERIC_STR(env, name_len, idx)) {
		zend_hash_index_update(ht, idx, &val);
	} else {
		php_register_variable_quick(env, name_len, &val, ht);
	}
}

void _php_import_environment_variables(zval *array_ptr)
{
	tsrm_env_lock();

#ifndef PHP_WIN32
	for (char **env = environ; env != NULL && *env != NULL; env++) {
		import_environment_variable(Z_ARRVAL_P(array_ptr), *env);
	}
#else
	wchar_t *environmentw = GetEnvironmentStringsW();
	for (wchar_t *envw = environmentw; envw != NULL && *envw; envw += wcslen(envw) + 1) {
		char *env = php_win32_cp_w_to_any(envw);
		if (env != NULL) {
			import_environment_variable(Z_ARRVAL_P(array_ptr), env);
			free(env);
		}
	}
	FreeEnvironmentStringsW(environmentw);
#endif

	tsrm_env_unlock();
}

void _php_load_environment_variables(zval *array_ptr)
{
	php_import_environment_variables(array_ptr);
}

bool php_std_auto_global_callback(char *name, uint32_t name_len)
{
	zend_printf("%s\n", name);
	return 0; /* don't rearm */
}

/* {{{ php_build_argv */
PHPAPI void php_build_argv(const char *s, zval *track_vars_array)
{
	zval arr, argc, tmp;
	int count = 0;

	if (!(SG(request_info).argc || track_vars_array)) {
		return;
	}

	array_init(&arr);

	/* Prepare argv */
	if (SG(request_info).argc) { /* are we in cli sapi? */
		int i;
		for (i = 0; i < SG(request_info).argc; i++) {
			ZVAL_STRING(&tmp, SG(request_info).argv[i]);
			if (zend_hash_next_index_insert(Z_ARRVAL(arr), &tmp) == NULL) {
				zend_string_efree(Z_STR(tmp));
			}
		}
	} else 	if (s && *s) {
		while (1) {
			const char *space = strchr(s, '+');
			/* auto-type */
			ZVAL_STRINGL(&tmp, s, space ? space - s : strlen(s));
			count++;
			if (zend_hash_next_index_insert(Z_ARRVAL(arr), &tmp) == NULL) {
				zend_string_efree(Z_STR(tmp));
			}
			if (!space) {
				break;
			}
			s = space + 1;
		}
	}

	/* prepare argc */
	if (SG(request_info).argc) {
		ZVAL_LONG(&argc, SG(request_info).argc);
	} else {
		ZVAL_LONG(&argc, count);
	}

	if (SG(request_info).argc) {
		Z_ADDREF(arr);
		zend_hash_update(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_ARGV), &arr);
		zend_hash_update(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_ARGC), &argc);
	}
	if (track_vars_array && Z_TYPE_P(track_vars_array) == IS_ARRAY) {
		Z_ADDREF(arr);
		zend_hash_update(Z_ARRVAL_P(track_vars_array), ZSTR_KNOWN(ZEND_STR_ARGV), &arr);
		zend_hash_update(Z_ARRVAL_P(track_vars_array), ZSTR_KNOWN(ZEND_STR_ARGC), &argc);
	}
	zval_ptr_dtor_nogc(&arr);
}
/* }}} */

/* {{{ php_register_server_variables */
static inline void php_register_server_variables(void)
{
	zval tmp;
	zval *arr = &PG(http_globals)[TRACK_VARS_SERVER];
	HashTable *ht;

	zval_ptr_dtor_nogc(arr);
	array_init(arr);

	/* Server variables */
	if (sapi_module.register_server_variables) {
		sapi_module.register_server_variables(arr);
	}
	ht = Z_ARRVAL_P(arr);

	/* PHP Authentication support */
	if (SG(request_info).auth_user) {
		ZVAL_STRING(&tmp, SG(request_info).auth_user);
		php_register_variable_quick("PHP_AUTH_USER", sizeof("PHP_AUTH_USER")-1, &tmp, ht);
	}
	if (SG(request_info).auth_password) {
		ZVAL_STRING(&tmp, SG(request_info).auth_password);
		php_register_variable_quick("PHP_AUTH_PW", sizeof("PHP_AUTH_PW")-1, &tmp, ht);
	}
	if (SG(request_info).auth_digest) {
		ZVAL_STRING(&tmp, SG(request_info).auth_digest);
		php_register_variable_quick("PHP_AUTH_DIGEST", sizeof("PHP_AUTH_DIGEST")-1, &tmp, ht);
	}

	/* store request init time */
	ZVAL_DOUBLE(&tmp, sapi_get_request_time());
	php_register_variable_quick("REQUEST_TIME_FLOAT", sizeof("REQUEST_TIME_FLOAT")-1, &tmp, ht);
	ZVAL_LONG(&tmp, zend_dval_to_lval(Z_DVAL(tmp)));
	php_register_variable_quick("REQUEST_TIME", sizeof("REQUEST_TIME")-1, &tmp, ht);
}
/* }}} */

/* {{{ php_autoglobal_merge */
static void php_autoglobal_merge(HashTable *dest, HashTable *src)
{
	zval *src_entry, *dest_entry;
	zend_string *string_key;
	zend_ulong num_key;
	int globals_check = (dest == (&EG(symbol_table)));

	ZEND_HASH_FOREACH_KEY_VAL(src, num_key, string_key, src_entry) {
		if (Z_TYPE_P(src_entry) != IS_ARRAY
			|| (string_key && (dest_entry = zend_hash_find(dest, string_key)) == NULL)
			|| (string_key == NULL && (dest_entry = zend_hash_index_find(dest, num_key)) == NULL)
			|| Z_TYPE_P(dest_entry) != IS_ARRAY) {
			Z_TRY_ADDREF_P(src_entry);
			if (string_key) {
				if (!globals_check || !zend_string_equals_literal(string_key, "GLOBALS")) {
					zend_hash_update(dest, string_key, src_entry);
				} else {
					Z_TRY_DELREF_P(src_entry);
				}
			} else {
				zend_hash_index_update(dest, num_key, src_entry);
			}
		} else {
			SEPARATE_ARRAY(dest_entry);
			php_autoglobal_merge(Z_ARRVAL_P(dest_entry), Z_ARRVAL_P(src_entry));
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ php_hash_environment */
PHPAPI int php_hash_environment(void)
{
	memset(PG(http_globals), 0, sizeof(PG(http_globals)));
	zend_activate_auto_globals();
	if (PG(register_argc_argv)) {
		php_build_argv(SG(request_info).query_string, &PG(http_globals)[TRACK_VARS_SERVER]);
	}
	return SUCCESS;
}
/* }}} */

static bool php_auto_globals_create_get(zend_string *name)
{
	if (PG(variables_order) && (strchr(PG(variables_order),'G') || strchr(PG(variables_order),'g'))) {
		sapi_module.treat_data(PARSE_GET, NULL, NULL);
	} else {
		zval_ptr_dtor_nogc(&PG(http_globals)[TRACK_VARS_GET]);
		array_init(&PG(http_globals)[TRACK_VARS_GET]);
	}

	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_GET]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_GET]);

	return 0; /* don't rearm */
}

static bool php_auto_globals_create_post(zend_string *name)
{
	if (PG(variables_order) &&
			(strchr(PG(variables_order),'P') || strchr(PG(variables_order),'p')) &&
		!SG(headers_sent) &&
		SG(request_info).request_method &&
		!strcasecmp(SG(request_info).request_method, "POST")) {
		sapi_module.treat_data(PARSE_POST, NULL, NULL);
	} else {
		zval_ptr_dtor_nogc(&PG(http_globals)[TRACK_VARS_POST]);
		array_init(&PG(http_globals)[TRACK_VARS_POST]);
	}

	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_POST]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_POST]);

	return 0; /* don't rearm */
}

static bool php_auto_globals_create_cookie(zend_string *name)
{
	if (PG(variables_order) && (strchr(PG(variables_order),'C') || strchr(PG(variables_order),'c'))) {
		sapi_module.treat_data(PARSE_COOKIE, NULL, NULL);
	} else {
		zval_ptr_dtor_nogc(&PG(http_globals)[TRACK_VARS_COOKIE]);
		array_init(&PG(http_globals)[TRACK_VARS_COOKIE]);
	}

	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_COOKIE]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_COOKIE]);

	return 0; /* don't rearm */
}

static bool php_auto_globals_create_files(zend_string *name)
{
	if (Z_TYPE(PG(http_globals)[TRACK_VARS_FILES]) == IS_UNDEF) {
		array_init(&PG(http_globals)[TRACK_VARS_FILES]);
	}

	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_FILES]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_FILES]);

	return 0; /* don't rearm */
}

/* Ugly hack to fix HTTP_PROXY issue, see bug #72573 */
static void check_http_proxy(HashTable *var_table)
{
	if (zend_hash_str_exists(var_table, "HTTP_PROXY", sizeof("HTTP_PROXY")-1)) {
		char *local_proxy = getenv("HTTP_PROXY");

		if (!local_proxy) {
			zend_hash_str_del(var_table, "HTTP_PROXY", sizeof("HTTP_PROXY")-1);
		} else {
			zval local_zval;
			ZVAL_STRING(&local_zval, local_proxy);
			zend_hash_str_update(var_table, "HTTP_PROXY", sizeof("HTTP_PROXY")-1, &local_zval);
		}
	}
}

static bool php_auto_globals_create_server(zend_string *name)
{
	if (PG(variables_order) && (strchr(PG(variables_order),'S') || strchr(PG(variables_order),'s'))) {
		php_register_server_variables();

		if (PG(register_argc_argv)) {
			if (SG(request_info).argc) {
				zval *argc, *argv;

				if ((argc = zend_hash_find_ex_ind(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_ARGC), 1)) != NULL &&
					(argv = zend_hash_find_ex_ind(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_ARGV), 1)) != NULL) {
					Z_ADDREF_P(argv);
					zend_hash_update(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), ZSTR_KNOWN(ZEND_STR_ARGV), argv);
					zend_hash_update(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), ZSTR_KNOWN(ZEND_STR_ARGC), argc);
				}
			} else {
				php_build_argv(SG(request_info).query_string, &PG(http_globals)[TRACK_VARS_SERVER]);
			}
		}

	} else {
		zval_ptr_dtor_nogc(&PG(http_globals)[TRACK_VARS_SERVER]);
		array_init(&PG(http_globals)[TRACK_VARS_SERVER]);
	}

	check_http_proxy(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]));
	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_SERVER]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_SERVER]);

	/* TODO: TRACK_VARS_SERVER is modified in a number of places (e.g. phar) past this point,
	 * where rc>1 due to the $_SERVER global. Ideally this shouldn't happen, but for now we
	 * ignore this issue, as it would probably require larger changes. */
	HT_ALLOW_COW_VIOLATION(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]));

	return 0; /* don't rearm */
}

static bool php_auto_globals_create_env(zend_string *name)
{
	zval_ptr_dtor_nogc(&PG(http_globals)[TRACK_VARS_ENV]);
	array_init(&PG(http_globals)[TRACK_VARS_ENV]);

	if (PG(variables_order) && (strchr(PG(variables_order),'E') || strchr(PG(variables_order),'e'))) {
		php_import_environment_variables(&PG(http_globals)[TRACK_VARS_ENV]);
	}

	check_http_proxy(Z_ARRVAL(PG(http_globals)[TRACK_VARS_ENV]));
	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_ENV]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_ENV]);

	return 0; /* don't rearm */
}

static bool php_auto_globals_create_request(zend_string *name)
{
	zval form_variables;
	unsigned char _gpc_flags[3] = {0, 0, 0};
	char *p;

	array_init(&form_variables);

	if (PG(request_order) != NULL) {
		p = PG(request_order);
	} else {
		p = PG(variables_order);
	}

	for (; p && *p; p++) {
		switch (*p) {
			case 'g':
			case 'G':
				if (!_gpc_flags[0]) {
					php_autoglobal_merge(Z_ARRVAL(form_variables), Z_ARRVAL(PG(http_globals)[TRACK_VARS_GET]));
					_gpc_flags[0] = 1;
				}
				break;
			case 'p':
			case 'P':
				if (!_gpc_flags[1]) {
					php_autoglobal_merge(Z_ARRVAL(form_variables), Z_ARRVAL(PG(http_globals)[TRACK_VARS_POST]));
					_gpc_flags[1] = 1;
				}
				break;
			case 'c':
			case 'C':
				if (!_gpc_flags[2]) {
					php_autoglobal_merge(Z_ARRVAL(form_variables), Z_ARRVAL(PG(http_globals)[TRACK_VARS_COOKIE]));
					_gpc_flags[2] = 1;
				}
				break;
		}
	}

	zend_hash_update(&EG(symbol_table), name, &form_variables);
	return 0;
}

void php_startup_auto_globals(void)
{
	zend_register_auto_global(zend_string_init_interned("_GET", sizeof("_GET")-1, 1), 0, php_auto_globals_create_get);
	zend_register_auto_global(zend_string_init_interned("_POST", sizeof("_POST")-1, 1), 0, php_auto_globals_create_post);
	zend_register_auto_global(zend_string_init_interned("_COOKIE", sizeof("_COOKIE")-1, 1), 0, php_auto_globals_create_cookie);
	zend_register_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_SERVER), PG(auto_globals_jit), php_auto_globals_create_server);
	zend_register_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_ENV), PG(auto_globals_jit), php_auto_globals_create_env);
	zend_register_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_REQUEST), PG(auto_globals_jit), php_auto_globals_create_request);
	zend_register_auto_global(zend_string_init_interned("_FILES", sizeof("_FILES")-1, 1), 0, php_auto_globals_create_files);
}
