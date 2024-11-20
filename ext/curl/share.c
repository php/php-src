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
   | Author: Pierrick Charron <pierrick@php.net>                          |
   +----------------------------------------------------------------------+
*/

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"

#include "curl_private.h"

#include <curl/curl.h>

#define SAVE_CURLSH_ERROR(__handle, __err) (__handle)->err.no = (int) __err;

/**
 * Free a persistent curl share handle.
 */
void curl_share_free_persistent(zval *data)
{
	CURLSH *handle = Z_PTR_P(data);

	if (!handle) {
		return;
	}

	curl_share_cleanup(handle);
}

/**
 * Initialize a share curl handle, optionally with share options and a persistent ID.
 */
PHP_FUNCTION(curl_share_init)
{
	zval *share_opts = NULL, *entry = NULL;
	zend_string *persistent_id = NULL;

	php_curlsh *sh;

	CURLSHcode error;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(share_opts)
		Z_PARAM_STR_OR_NULL(persistent_id)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, curl_share_ce);
	sh = Z_CURL_SHARE_P(return_value);

	if (persistent_id) {
		zval *persisted = zend_hash_find(&CURL_G(persistent_share_handles), persistent_id);

		if (persisted) {
			sh->share         = Z_PTR_P(persisted);
			sh->persistent_id = zend_string_copy(persistent_id);

			return;
		}
	}

	// The user did not provide a persistent share ID, or we could not find an existing share handle, so we'll have to
	// create one.
	sh->share = curl_share_init();

	if (share_opts) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(share_opts), entry) {
			ZVAL_DEREF(entry);

			error = curl_share_setopt(sh->share, CURLSHOPT_SHARE, zval_get_long_ex(entry, true));

			if (error != CURLSHE_OK) {
				zend_throw_exception_ex(
					NULL,
					0,
					"Could not construct cURL share handle: %s",
					curl_share_strerror(error)
				);

				goto error;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (persistent_id) {
		// It's important not to mark this as persistent until *after* we've successfully set all of the share options,
		// as otherwise the curl_share_free_obj in the error handler won't free the CURLSH.
		sh->persistent_id = zend_string_copy(persistent_id);

		// We use the zend_hash_str form here, since we want to ensure it allocates a new persistent string, instead of
		// using the request-bound persistent_id.
		zend_hash_str_add_new_ptr(
			&CURL_G(persistent_share_handles),
			ZSTR_VAL(persistent_id),
			ZSTR_LEN(persistent_id),
			sh->share
		);
	}

	return;

 error:
	zval_ptr_dtor(return_value);

	RETURN_THROWS();
}

/**
 * Close a persistent curl share handle. NOP for non-persistent share handles.
 */
PHP_FUNCTION(curl_share_close)
{
	zval *z_sh;

	php_curlsh *sh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT_OF_CLASS(z_sh, curl_share_ce)
	ZEND_PARSE_PARAMETERS_END();

	sh = Z_CURL_SHARE_P(z_sh);

	if (sh->persistent_id) {
		// Deleting the share from the hash table will trigger curl_share_free_persistent, so no need to call it here.
		zend_hash_del(&CURL_G(persistent_share_handles), sh->persistent_id);
	}
}

static bool _php_curl_share_setopt(php_curlsh *sh, zend_long option, zval *zvalue, zval *return_value) /* {{{ */
{
	CURLSHcode error = CURLSHE_OK;

	switch (option) {
		case CURLSHOPT_SHARE:
		case CURLSHOPT_UNSHARE:
			error = curl_share_setopt(sh->share, option, zval_get_long(zvalue));
			break;

		default:
			zend_argument_value_error(2, "is not a valid cURL share option");
			error = CURLSHE_BAD_OPTION;
			break;
	}

	SAVE_CURLSH_ERROR(sh, error);

	return error == CURLSHE_OK;
}

/**
 * Set an option for a cURL transfer.
 */
PHP_FUNCTION(curl_share_setopt)
{
	zval       *z_sh, *zvalue;
	zend_long  options;
	php_curlsh *sh;

	ZEND_PARSE_PARAMETERS_START(3,3)
		Z_PARAM_OBJECT_OF_CLASS(z_sh, curl_share_ce)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	sh = Z_CURL_SHARE_P(z_sh);

	if (_php_curl_share_setopt(sh, options, zvalue, return_value)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/**
 * Return an integer containing the last share curl error number.
 */
PHP_FUNCTION(curl_share_errno)
{
	zval       *z_sh;
	php_curlsh *sh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT_OF_CLASS(z_sh, curl_share_ce)
	ZEND_PARSE_PARAMETERS_END();

	sh = Z_CURL_SHARE_P(z_sh);

	RETURN_LONG(sh->err.no);
}


/**
 * Return a string describing the error code.
 */
PHP_FUNCTION(curl_share_strerror)
{
	zend_long code;
	const char *str;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_LONG(code)
	ZEND_PARSE_PARAMETERS_END();

	str = curl_share_strerror(code);
	if (str) {
		RETURN_STRING(str);
	} else {
		RETURN_NULL();
	}
}

/* CurlShareHandle class */

static zend_object *curl_share_create_object(zend_class_entry *class_type) {
	php_curlsh *intern = zend_object_alloc(sizeof(php_curlsh), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

static zend_function *curl_share_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct CurlShareHandle, use curl_share_init() instead");
	return NULL;
}

void curl_share_free_obj(zend_object *object)
{
	php_curlsh *sh = curl_share_from_obj(object);

	if (!sh->persistent_id) {
		curl_share_cleanup(sh->share);
	} else {
		zend_string_release(sh->persistent_id);
	}

	zend_object_std_dtor(&sh->std);
}

static zend_object_handlers curl_share_handlers;

void curl_share_register_handlers(void) {
	curl_share_ce->create_object = curl_share_create_object;
	curl_share_ce->default_object_handlers = &curl_share_handlers;

	memcpy(&curl_share_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	curl_share_handlers.offset = XtOffsetOf(php_curlsh, std);
	curl_share_handlers.free_obj = curl_share_free_obj;
	curl_share_handlers.get_constructor = curl_share_get_constructor;
	curl_share_handlers.clone_obj = NULL;
	curl_share_handlers.compare = zend_objects_not_comparable;
}
