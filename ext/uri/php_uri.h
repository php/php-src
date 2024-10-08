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

ZEND_BEGIN_MODULE_GLOBALS(uri)
	zend_string *default_handler_name;
	uri_handler_t *default_handler;
ZEND_END_MODULE_GLOBALS(uri)

#if defined(ZTS) && defined(COMPILE_DL_URI)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(uri)

#define URI_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(uri, v)

PHPAPI uri_handler_t *php_uri_get_handler(const zend_string *handler_name);
PHPAPI void *php_uri_parse(uri_handler_t *uri_handler, zend_string *uri_str);
PHPAPI zend_result php_uri_get_scheme(const uri_handler_t *uri_handler, void *uri, zval *zv);
PHPAPI zend_result php_uri_get_user(const uri_handler_t *uri_handler, void *uri, zval *zv);
PHPAPI zend_result php_uri_get_password(const uri_handler_t *uri_handler, void *uri, zval *zv);
PHPAPI zend_result php_uri_get_host(const uri_handler_t *uri_handler, void *uri, zval *zv);
PHPAPI zend_result php_uri_get_port(const uri_handler_t *uri_handler, void *uri, zval *zv);
PHPAPI zend_result php_uri_get_path(const uri_handler_t *uri_handler, void *uri, zval *zv);
PHPAPI zend_result php_uri_get_query(const uri_handler_t *uri_handler, void *uri, zval *zv);
PHPAPI zend_result php_uri_get_fragment(const uri_handler_t *uri_handler, void *uri, zval *zv);
PHPAPI void php_uri_free(uri_handler_t *uri_handler, void *uri);

#endif
