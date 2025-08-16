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
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_URI_H
#define PHP_URI_H

#include "php_uri_common.h"

extern zend_module_entry uri_module_entry;
#define phpext_uri_ptr &uri_module_entry

typedef struct php_uri {
	zend_string *scheme;
	zend_string *user;
	zend_string *password;
	zend_string *host;
	unsigned short port;
	zend_string *path;
	zend_string *query;
	zend_string *fragment;
} php_uri;

/**
 * Registers a URI handler. The handler must have a unique name.
 *
 * @param uri_handler The URI handler
 * @return SUCCESS in case of success, FAILURE otherwise
 */
PHPAPI zend_result uri_handler_register(const uri_handler_t *uri_handler);

/**
 * Returns the registered URI handler based on uri_handler_name.
 *
 * @param uri_handler_name The URI handler name
 * @return The URI handler
 */
PHPAPI uri_handler_t *php_uri_get_handler(const zend_string *uri_handler_name);

ZEND_ATTRIBUTE_NONNULL PHPAPI uri_internal_t *php_uri_parse(const uri_handler_t *uri_handler, const char *uri_str, size_t uri_str_len, bool silent);

/**
 * Retrieves the scheme component based on the read_mode and passes it to the zv ZVAL in case of success.
 * 
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param internal_uri The internal URI whose uri member is used to retrieve the component
 * @param read_mode The read mode
 * @param zv The output parameter containing the retrieved component as a ZVAL (either IS_STRING or IS_NULL).
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_scheme(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv);

/**
 * Retrieves the username component based on the read_mode and passes it to the zv ZVAL in case of success.
 *
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param internal_uri The internal URI whose uri member is used to retrieve the component
 * @param read_mode The read mode
 * @param zv The output parameter containing the retrieved component as a ZVAL (either IS_STRING or IS_NULL).
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_username(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv);

/**
 * Retrieves the password component based on the read_mode and passes it to the zv ZVAL in case of success.
 *
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param internal_uri The internal URI whose uri member is used to retrieve the component
 * @param read_mode The read mode
 * @param zv The output parameter containing the retrieved component as a ZVAL (either IS_STRING or IS_NULL).
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_password(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv);

/**
 * Retrieves the host component based on the read_mode and passes it to the zv ZVAL in case of success.
 *
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param internal_uri The internal URI whose uri member is used to retrieve the component
 * @param read_mode The read mode
 * @param zv The output parameter containing the retrieved component as a ZVAL (either IS_STRING or IS_NULL).
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_host(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv);

/**
 * Retrieves the port component based on the read_mode and passes it to the zv ZVAL in case of success.
 *
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param internal_uri The internal URI whose uri member is used to retrieve the component
 * @param read_mode The read mode
 * @param zv The output parameter containing the retrieved component as a ZVAL (either IS_LONG or IS_NULL).
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_port(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv);

/**
 * Retrieves the path component based on the read_mode and passes it to the zv ZVAL in case of success.
 *
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param internal_uri The internal URI whose uri member is used to retrieve the component
 * @param read_mode The read mode
 * @param zv The output parameter containing the retrieved component as a ZVAL (either IS_STRING or IS_NULL).
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_path(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv);

/**
 * Retrieves the query component based on the read_mode and passes it to the zv ZVAL in case of success.
 *
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param internal_uri The internal URI whose uri member is used to retrieve the component
 * @param read_mode The read mode
 * @param zv The output parameter containing the retrieved component as a ZVAL (either IS_STRING or IS_NULL).
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_query(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv);

/**
 * Retrieves the fragment component based on the read_mode and passes it to the zv ZVAL in case of success.
 *
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param internal_uri The internal URI whose uri member is used to retrieve the component
 * @param read_mode The read mode
 * @param zv The output parameter containing the retrieved component as a ZVAL (either IS_STRING or IS_NULL).
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_fragment(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv);

/**
 * Frees the uri member within the provided internal URI.
 *
 * @param internal_uri The internal URI
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI void php_uri_free(uri_internal_t *internal_uri);

/**
 * Creates a new php_uri struct containing all the URI components. The components are retrieved based on the read_mode parameter.
 *
 * Read_mode can be one of the following:
 * - URI_COMPONENT_READ_RAW: Retrieves the raw, non-normalized variant of the URI component
 * - URI_COMPONENT_READ_NORMALIZED_ASCII: Retrieves the normalized variant of the requested URI component that must only contain ASCII characters
 * - URI_COMPONENT_READ_NORMALIZED_UNICODE: Retrieves the normalized variant of the requested URI component that may contain Unicode codepoints
 *
 * @param uri_handler The URI handler whose parse_uri() handler is called
 * @param uri_str The input string that is going to be parsed
 * @param uri_str_len Length of the input string
 * @param read_mode The read mode based on which components are retrieved
 * @param silent Whether to throw a Uri\InvalidUriException in case of failure
 * @return The created php_uri struct in case of success, NULL otherwise
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI php_uri *php_uri_parse_to_struct(
	const uri_handler_t *uri_handler, const char *uri_str, size_t uri_str_len, uri_component_read_mode_t read_mode, bool silent
);

/**
 * Frees the provided php_uri struct.
 *
 * @param uri The php_uri struct to free
 */
ZEND_ATTRIBUTE_NONNULL PHPAPI void php_uri_struct_free(php_uri *uri);

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 2) PHPAPI void php_uri_instantiate_uri(
	INTERNAL_FUNCTION_PARAMETERS, const uri_handler_t *handler, const zend_string *uri_str, const zend_object *base_url_object,
	bool should_throw, bool should_update_this_object, zval *errors_zv
);

ZEND_ATTRIBUTE_NONNULL PHPAPI void php_uri_implementation_set_object_handlers(zend_class_entry *ce, zend_object_handlers *object_handlers);

#endif
