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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_attributes.h"
#include "main/php_ini.h"

#include "php_uri.h"
#include "php_uri_arginfo.h"

zend_class_entry *uri_exception_ce;
zend_class_entry *invalid_uri_exception_ce;
zend_class_entry *whatwg_invalid_url_exception_ce;

#define URIPARSER_VERSION "0.9.8"

static const zend_module_dep uri_deps[] = {
	ZEND_MOD_REQUIRED("lexbor")
	ZEND_MOD_END
};


static PHP_MINIT_FUNCTION(uri)
{
	uri_exception_ce = register_class_Uri_UriException(zend_ce_exception);
	invalid_uri_exception_ce = register_class_Uri_InvalidUriException(uri_exception_ce);
	whatwg_invalid_url_exception_ce = register_class_Uri_WhatWg_InvalidUrlException(invalid_uri_exception_ce);

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(uri)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "uri support", "active");
	php_info_print_table_row(2, "uriparser version", URIPARSER_VERSION);
	php_info_print_table_end();
}

static PHP_MSHUTDOWN_FUNCTION(uri)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

PHP_RINIT_FUNCTION(uri)
{
#if defined(COMPILE_DL_URI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(uri)
{
	return SUCCESS;
}

zend_module_entry uri_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	uri_deps,
	"uri",                          /* Extension name */
	NULL,                           /* zend_function_entry */
	PHP_MINIT(uri),                 /* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(uri),             /* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(uri),                 /* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(uri),             /* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(uri),                 /* PHP_MINFO - Module info */
	PHP_VERSION,                    /* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_URI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(uri)
#endif
