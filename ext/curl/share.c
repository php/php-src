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

/* {{{ Initialize a share curl handle */
PHP_FUNCTION(curl_share_init)
{
	php_curlsh *sh;

	ZEND_PARSE_PARAMETERS_NONE();

	object_init_ex(return_value, curl_share_ce);
	sh = Z_CURL_SHARE_P(return_value);

	sh->share = curl_share_init();
}
/* }}} */

/* {{{ Close a set of cURL handles */
PHP_FUNCTION(curl_share_close)
{
	zval *z_sh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT_OF_CLASS(z_sh, curl_share_ce)
	ZEND_PARSE_PARAMETERS_END();
}
/* }}} */

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
/* }}} */

/* {{{ Set an option for a cURL transfer */
PHP_FUNCTION(curl_share_setopt)
{
	zval       *z_sh, *zvalue;
	zend_long        options;
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
/* }}} */

/* {{{ Return an integer containing the last share curl error number */
PHP_FUNCTION(curl_share_errno)
{
	zval        *z_sh;
	php_curlsh  *sh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT_OF_CLASS(z_sh, curl_share_ce)
	ZEND_PARSE_PARAMETERS_END();

	sh = Z_CURL_SHARE_P(z_sh);

	RETURN_LONG(sh->err.no);
}
/* }}} */


/* {{{ return string describing error code */
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
/* }}} */

/**
 * Initialize a persistent curl share handle with the given share options, reusing an existing one if found.
 *
 * Throws an exception if the share options are invalid.
 */
PHP_FUNCTION(curl_persistent_share_init)
{
	zval *share_opts = NULL, *entry = NULL;
	zend_ulong persistent_id = 0;

	php_curlsh *sh;

	CURLSHcode error;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(share_opts)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, curl_persistent_share_ce);
	sh = Z_CURL_SHARE_P(return_value);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(share_opts), entry) {
		ZVAL_DEREF(entry);

		zend_ulong option = zval_get_long_ex(entry, true);

		if (option == CURL_LOCK_DATA_COOKIE) {
			zend_throw_exception_ex(
				NULL,
				0,
				"CURL_LOCK_DATA_COOKIE is not allowed with persistent curl share handles"
			);

			goto error;
		}

		// Ensure that each additional option results in a unique persistent ID.
		persistent_id += 1 << option;
	} ZEND_HASH_FOREACH_END();

	if (persistent_id) {
		zval *persisted = zend_hash_index_find(&CURL_G(persistent_curlsh), persistent_id);

		if (persisted) {
			sh->share = Z_PTR_P(persisted);

			return;
		}
	}

	// We could not find an existing share handle, so we'll have to create one.
	sh->share = curl_share_init();

	// Apply $share_options to the handle.
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(share_opts), entry) {
		ZVAL_DEREF(entry);

		error = curl_share_setopt(sh->share, CURLSHOPT_SHARE, zval_get_long(entry));

		if (error != CURLSHE_OK) {
			zend_throw_exception_ex(
				NULL,
				0,
				"Could not construct persistent cURL share handle: %s",
				curl_share_strerror(error)
			);

			goto error;
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_index_add_new_ptr(
		&CURL_G(persistent_curlsh),
		persistent_id,
		sh->share
	);

	return;

 error:
	if (sh->share) {
		curl_share_cleanup(sh->share);
	}

	RETURN_THROWS();
}

/**
 * Free a persistent curl share handle from the module global HashTable.
 *
 * See PHP_GINIT_FUNCTION in ext/curl/interface.c.
 */
void curl_share_free_persistent_curlsh(zval *data)
{
	CURLSH *handle = Z_PTR_P(data);

	if (!handle) {
		return;
	}

	curl_share_cleanup(handle);
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

	curl_share_cleanup(sh->share);
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

/* CurlPersistentShareHandle class */

static zend_function *curl_persistent_share_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct CurlPersistentShareHandle, use curl_persistent_share_init() instead");
	return NULL;
}

static zend_object_handlers curl_persistent_share_handlers;

void curl_persistent_share_register_handlers(void) {
	curl_persistent_share_ce->create_object = curl_share_create_object;
	curl_persistent_share_ce->default_object_handlers = &curl_persistent_share_handlers;

	memcpy(&curl_persistent_share_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	curl_persistent_share_handlers.offset = XtOffsetOf(php_curlsh, std);
	curl_persistent_share_handlers.get_constructor = curl_persistent_share_get_constructor;
	curl_persistent_share_handlers.clone_obj = NULL;
	curl_persistent_share_handlers.compare = zend_objects_not_comparable;
}
