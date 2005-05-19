/*
  $Id$
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_variables.h"

#include "filter.h"

ZEND_DECLARE_MODULE_GLOBALS(filter)

#ifndef PARSE_ENV
#define PARSE_ENV 4
#endif

#ifndef PARSE_SERVER
#define PARSE_SERVER 5
#endif

/* {{{ filter_functions[]
 */
function_entry filter_functions[] = {
	PHP_FE(filter,	NULL)
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
    "0.1",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FILTER
ZEND_GET_MODULE(filter)
#endif

/* {{{ UpdateDefaultFilter
 */
static PHP_INI_MH(UpdateDefaultFilter) {
	if(!strcasecmp(new_value, "notags")) {
		IF_G(default_filter) = NOTAGS;
	}
	else
	if(!strcasecmp(new_value, "raw")) {
		IF_G(default_filter) = F_UNSAFE_RAW;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("filter.default", "notags", PHP_INI_ALL, UpdateDefaultFilter, default_filter, zend_filter_globals, filter_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_filter_init_globals
 */
static void php_filter_init_globals(zend_filter_globals *filter_globals)
{
	filter_globals->post_array = NULL;
	filter_globals->get_array = NULL;
	filter_globals->cookie_array = NULL;
	filter_globals->env_array = NULL;
	filter_globals->server_array = NULL;
	filter_globals->default_filter = NOTAGS;
}
/* }}} */

#define PARSE_REQUEST 99

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(filter)
{
	ZEND_INIT_MODULE_GLOBALS(filter, php_filter_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("FILTER_POST", PARSE_POST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_GET", PARSE_GET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_COOKIE", PARSE_COOKIE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_REQUEST", PARSE_REQUEST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_ENV", PARSE_ENV, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SERVER", PARSE_SERVER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_NONE", FILTER_FLAG_NONE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_STRIP_LOW", FILTER_FLAG_STRIP_LOW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_STRIP_HIGH", FILTER_FLAG_STRIP_HIGH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_COOK_LOW", FILTER_FLAG_COOK_LOW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_COOK_HIGH", FILTER_FLAG_COOK_HIGH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_SIGN", FILTER_FLAG_ALLOW_SIGN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_FRACTION", FILTER_FLAG_ALLOW_FRACTION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_THOUSAND", FILTER_FLAG_ALLOW_THOUSAND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ENCODE_AMP", FILTER_FLAG_ENCODE_AMP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_HTML_NO_TAGS", FILTER_HTML_NO_TAGS, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_UNSAFE_RAW", F_UNSAFE_RAW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_STRIPPED", F_STRIPPED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_COOKED", F_COOKED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_HTML", F_HTML, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_EMAIL", F_EMAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_URL", F_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_NUMBER", F_NUMBER, CONST_CS | CONST_PERSISTENT);

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
PHP_RSHUTDOWN_FUNCTION(filter)
{
	if(IF_G(get_array)) {
		zval_ptr_dtor(&IF_G(get_array));
		IF_G(get_array) = NULL;
	}
	if(IF_G(post_array)) {
		zval_ptr_dtor(&IF_G(post_array));
		IF_G(post_array) = NULL;
	}
	if(IF_G(cookie_array)) {
		zval_ptr_dtor(&IF_G(cookie_array));
		IF_G(cookie_array) = NULL;
	}
	if(IF_G(env_array)) {
		zval_ptr_dtor(&IF_G(env_array));
		IF_G(env_array) = NULL;
	}
	if(IF_G(server_array)) {
		zval_ptr_dtor(&IF_G(server_array));
		IF_G(server_array) = NULL;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(filter)
{
	char tmp[256];

	php_info_print_table_start();
	php_info_print_table_row( 2, "PHP extension for Input Validation and Filtering", "enabled" );
	php_info_print_table_row( 2, "Revision", "$Revision$");
	sprintf(tmp, "%d",IF_G(default_filter));
	php_info_print_table_row( 2, "default_filter", tmp);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned *new_val_len)
 */
unsigned int  php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC)
{
	zval new_var;
	zval *array_ptr = NULL;
	char *raw_var, *out;
	int var_len, res, ol, out_len;

	assert(*val != NULL);

#if PHP_API_VERSION > 20041224
	if(IF_G(default_filter)==F_UNSAFE_RAW) {
		if(new_val_len) *new_val_len = val_len;
		return 1;
	}
#else
	if(IF_G(default_filter)==F_UNSAFE_RAW) return(val_len);
#endif

	switch(arg) {
		case PARSE_GET:
			if(!IF_G(get_array)) {
				ALLOC_ZVAL(array_ptr);
				array_init(array_ptr);
				INIT_PZVAL(array_ptr);
				IF_G(get_array) = array_ptr;
			}
			else {
				array_ptr = IF_G(get_array);
			}
			break;
		case PARSE_POST:
			if(!IF_G(post_array)) {
				ALLOC_ZVAL(array_ptr);
				array_init(array_ptr);
				INIT_PZVAL(array_ptr);
				IF_G(post_array) = array_ptr;
			}
			else {
				array_ptr = IF_G(post_array);
			}
			break;
		case PARSE_COOKIE:
			if(!IF_G(cookie_array)) {
				ALLOC_ZVAL(array_ptr);
				array_init(array_ptr);
				INIT_PZVAL(array_ptr);
				IF_G(cookie_array) = array_ptr;
			}
			else {
				array_ptr = IF_G(cookie_array);
			}
			break;
		case PARSE_ENV:
			if(!IF_G(env_array)) {
				ALLOC_ZVAL(array_ptr);
				array_init(array_ptr);
				INIT_PZVAL(array_ptr);
				IF_G(env_array) = array_ptr;
			}
			else {
				array_ptr = IF_G(env_array);
			}
			break;
		case PARSE_SERVER:
			if(!IF_G(server_array)) {
				ALLOC_ZVAL(array_ptr);
				array_init(array_ptr);
				INIT_PZVAL(array_ptr);
				IF_G(server_array) = array_ptr;
			}
			else {
				array_ptr = IF_G(server_array);
			}
			break;
	}

	Z_STRLEN(new_var) = val_len;
	Z_STRVAL(new_var) = estrndup(*val, val_len);
	Z_TYPE(new_var) = IS_STRING;

	var_len = strlen(var);
	raw_var = emalloc(var_len+5);  /* RAW_ and a \0 */
	strcpy(raw_var, "RAW_");
	strlcat(raw_var,var,var_len+5);

	php_register_variable_ex(raw_var, &new_var, array_ptr TSRMLS_DC);

	ol = 0;
	out_len = val_len * 2;
	if(!out_len) out = estrdup("");
	else out = emalloc(out_len);
	while((res = php_filter_get_html(*val, val_len, out, &out_len, FILTER_HTML_NO_TAGS, FILTER_FLAG_ENCODE_AMP, NULL)) == FILTER_RESULT_OUTLEN_SMALL) {
		efree(out);
		ol++;
		out_len *= ol; /* Just in case we don't actually get the right out_len for some reason */
		out = emalloc(out_len);
	}
	*val = out;
#if PHP_API_VERSION > 20041224
	if(new_val_len) *new_val_len = out_len?out_len-1:0;
	return 1;
#else
	return(out_len?out_len-1:0);
#endif
}
/* }}} */

/* {{{ static void filter_recursive(zval *array, long filter, long flags, char *charset TSRMLS_DC)
 */
static void filter_recursive(zval *array, long filter, long flags, char *charset TSRMLS_DC)
{
	zval **element;
	HashPosition pos;
	int out_len, res, ol=0;  /* Yes, ol should start at 0 here because the filter returns the right length */
	char *out;

	if (Z_TYPE_P(array) == IS_ARRAY) {
		for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
			 zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &element, &pos) == SUCCESS;
			 zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
				filter_recursive(*element, filter, flags, charset TSRMLS_CC);
		}
	} else if(Z_STRLEN_P(array)) {
		switch(filter) {
			case F_STRIPPED:
				out_len = Z_STRLEN_P(array) + 1;
				out = emalloc(out_len);
				while((res = php_filter_get_stripped(Z_STRVAL_P(array), Z_STRLEN_P(array), out, &out_len, flags, charset)) == FILTER_RESULT_OUTLEN_SMALL) {
					efree(out);
					ol++;
					out_len *= ol;
					out = emalloc(out_len);
				}
				efree(Z_STRVAL_P(array));
				Z_STRVAL_P(array) = out;
				Z_STRLEN_P(array) = out_len - 1;
				break;

			case F_COOKED:
				out_len = Z_STRLEN_P(array) * 2;
				out = emalloc(out_len);
				while((res = php_filter_get_cooked(Z_STRVAL_P(array), Z_STRLEN_P(array), out, &out_len, flags, charset)) == FILTER_RESULT_OUTLEN_SMALL) {
					efree(out);
					ol++;
					out_len *= ol;
					out = emalloc(out_len);
				}
				efree(Z_STRVAL_P(array));
				Z_STRVAL_P(array) = out;
				Z_STRLEN_P(array) = out_len - 1;
				break;

			case F_EMAIL:
				out_len = Z_STRLEN_P(array) + 1;
				out = emalloc(out_len);
				while((res = php_filter_get_email(Z_STRVAL_P(array), Z_STRLEN_P(array), out, &out_len, flags, charset)) == FILTER_RESULT_OUTLEN_SMALL) {
					efree(out);
					ol++;
					out_len *= ol;
					out = emalloc(out_len);
				}
				if(res==FILTER_RESULT_BAD_IN) { Z_TYPE_P(array) = IS_BOOL; Z_LVAL_P(array) = 0; }
				else {
					efree(Z_STRVAL_P(array));
					Z_STRVAL_P(array) = out;
					Z_STRLEN_P(array) = out_len - 1;
				}
				break;

			case F_URL:
				out_len = Z_STRLEN_P(array) + 1;
				out = emalloc(out_len);
				while((res = php_filter_get_url(Z_STRVAL_P(array), Z_STRLEN_P(array), out, &out_len, flags, charset)) == FILTER_RESULT_OUTLEN_SMALL) {
					efree(out);
					ol++;
					out_len *= ol;
					out = emalloc(out_len);
				}
				if(res==FILTER_RESULT_BAD_IN) { Z_TYPE_P(array) = IS_BOOL; Z_LVAL_P(array) = 0; }
				else {
					efree(Z_STRVAL_P(array));
					Z_STRVAL_P(array) = out;
					Z_STRLEN_P(array) = out_len - 1;
				}
				break;

			case F_NUMBER:
				out_len = Z_STRLEN_P(array) + 1;
				out = emalloc(out_len);
				while((res = php_filter_get_number(Z_STRVAL_P(array), Z_STRLEN_P(array), out, &out_len, flags, charset)) == FILTER_RESULT_OUTLEN_SMALL) {
					efree(out);
					ol++;
					out_len *= ol;
					out = emalloc(out_len);
				}
				if(res==FILTER_RESULT_BAD_IN) { Z_TYPE_P(array) = IS_BOOL; Z_LVAL_P(array) = 0; }
				else {
					efree(Z_STRVAL_P(array));
					Z_STRVAL_P(array) = out;
					Z_STRLEN_P(array) = out_len - 1;
				}
				break;

			case F_NOTAGS:
			default:
				out_len = Z_STRLEN_P(array) + 1;
				out = emalloc(out_len);
				while((res = php_filter_get_html(Z_STRVAL_P(array), Z_STRLEN_P(array), out, &out_len, FILTER_HTML_NO_TAGS, flags, charset)) == FILTER_RESULT_OUTLEN_SMALL) {
					efree(out);
					ol++;
					out_len *= ol;
					out = emalloc(out_len);
				}
				efree(Z_STRVAL_P(array));
				Z_STRVAL_P(array) = out;
				Z_STRLEN_P(array) = out_len - 1;
		}
	}
}
/* }}} */

/* {{{ filter(constant type, string variable_name [, int filter [, int flags [, string charset]]])
 */
PHP_FUNCTION(filter)
{
	long arg, filter = F_NOTAGS, flags = 0;
	char *var;
	int var_len, charset_len, found = 0;
	int argc = ZEND_NUM_ARGS();
    zval **tmp;
	zval *array_ptr = NULL, *array_ptr2 = NULL, *array_ptr3 = NULL;
	HashTable *hash_ptr;
	char *raw_var, *charset = NULL;

	if(zend_parse_parameters(argc TSRMLS_CC, "ls|lls", &arg, &var, &var_len, &filter, &flags, &charset, &charset_len) == FAILURE) {
		return;
	}

	switch(arg) {
		case PARSE_GET:
			if(IF_G(default_filter)!=F_UNSAFE_RAW) array_ptr = IF_G(get_array);
			else array_ptr = PG(http_globals)[TRACK_VARS_GET];
			break;
		case PARSE_POST:
			if(IF_G(default_filter)!=F_UNSAFE_RAW) array_ptr = IF_G(post_array);
			else array_ptr = PG(http_globals)[TRACK_VARS_POST];
			break;
		case PARSE_COOKIE:
			if(IF_G(default_filter)!=F_UNSAFE_RAW) array_ptr = IF_G(cookie_array);
			else array_ptr = PG(http_globals)[TRACK_VARS_COOKIE];
			break;
		case PARSE_ENV:
			if(IF_G(default_filter)!=F_UNSAFE_RAW) array_ptr = IF_G(env_array);
			else array_ptr = PG(http_globals)[TRACK_VARS_ENV];
			break;
		case PARSE_SERVER:
			if(IF_G(default_filter)!=F_UNSAFE_RAW) array_ptr = IF_G(server_array);
			else array_ptr = PG(http_globals)[TRACK_VARS_SERVER];
			break;
		case PARSE_REQUEST:
			if (PG(variables_order)) {
				zval **a_ptr = &array_ptr;
				char *p, *variables_order = PG(variables_order);
				for (p=variables_order; p && *p; p++) {
					switch(*p) {
						case 'p':
						case 'P':
							if(IF_G(default_filter)!=F_UNSAFE_RAW) *a_ptr = IF_G(post_array);
							else *a_ptr = PG(http_globals)[TRACK_VARS_POST];
							break;
						case 'g':
						case 'G':
							if(IF_G(default_filter)!=F_UNSAFE_RAW) *a_ptr = IF_G(get_array);
							else *a_ptr = PG(http_globals)[TRACK_VARS_GET];
							break;
						case 'c':
						case 'C':
							if(IF_G(default_filter)!=F_UNSAFE_RAW) *a_ptr = IF_G(cookie_array);
							else *a_ptr = PG(http_globals)[TRACK_VARS_COOKIE];
							break;
					}
					if(array_ptr && !array_ptr2) { a_ptr = &array_ptr2; continue; }
					if(array_ptr2 && !array_ptr3) { a_ptr = &array_ptr3; }
				}
			} else {
				if(IF_G(default_filter)!=F_UNSAFE_RAW) array_ptr = IF_G(get_array);
				else array_ptr = PG(http_globals)[TRACK_VARS_GET];
				break;
			}

	}

	if(!array_ptr) RETURN_FALSE;

	if(IF_G(default_filter)!=F_UNSAFE_RAW) {
	/*
	 * I'm changing the variable name here because when running with register_globals on,
	 * the variable will end up in the global symbol table and I am using that var name
	 * in the internal raw storage arrays as well.
	 */
		var_len += 5;
		raw_var = emalloc(var_len);  /* RAW_ and a \0 */
		strcpy(raw_var, "RAW_");
		strlcat(raw_var,var,var_len);
	} else {
		raw_var = var;
		var_len++;
	}

	if(array_ptr3) {
		hash_ptr = HASH_OF(array_ptr3);
		if(hash_ptr && zend_hash_find(hash_ptr, raw_var, var_len, (void **)&tmp) == SUCCESS) {
			*return_value = **tmp;
			found = 1;
		} 
	}

	if(array_ptr2 && !found) {
		hash_ptr = HASH_OF(array_ptr2);
		if(hash_ptr && zend_hash_find(hash_ptr, raw_var, var_len, (void **)&tmp) == SUCCESS) {
			*return_value = **tmp;
			found = 1;
		}
	}

	if(!found) {
		hash_ptr = HASH_OF(array_ptr);

		if(hash_ptr && zend_hash_find(hash_ptr, raw_var, var_len, (void **)&tmp) == SUCCESS) {
			*return_value = **tmp;
			found = 1;
		}
	}

	if(found) {
		zval_copy_ctor(return_value);  /* Watch out for empty strings */
		if(filter != F_UNSAFE_RAW) {
			filter_recursive(return_value, filter, flags, charset);
		}
	} else {
		RETVAL_FALSE;
	}

	if(IF_G(default_filter)!=F_UNSAFE_RAW) {
		efree(raw_var);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
  vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
