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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_COM_DOTNET_H
#define PHP_COM_DOTNET_H

extern zend_module_entry com_dotnet_module_entry;
#define phpext_com_dotnet_ptr &com_dotnet_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_COM_DOTNET_API __declspec(dllexport)

#include "php_version.h"
#define PHP_COM_DOTNET_VERSION PHP_VERSION

PHP_MINIT_FUNCTION(com_dotnet);
PHP_MSHUTDOWN_FUNCTION(com_dotnet);
PHP_RINIT_FUNCTION(com_dotnet);
PHP_RSHUTDOWN_FUNCTION(com_dotnet);
PHP_MINFO_FUNCTION(com_dotnet);

ZEND_BEGIN_MODULE_GLOBALS(com_dotnet)
	bool allow_dcom;
	bool autoreg_verbose;
	bool autoreg_on;
	bool autoreg_case_sensitive;
	void *dotnet_runtime_stuff; /* opaque to avoid cluttering up other modules */
	int code_page; /* default code_page if left unspecified */
	bool rshutdown_started;
ZEND_END_MODULE_GLOBALS(com_dotnet)

#if defined(ZTS) && defined(COMPILE_DL_COM_DOTNET)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(com_dotnet)
#define COMG(v) ZEND_MODULE_GLOBALS_ACCESSOR(com_dotnet, v)

#endif	/* PHP_COM_DOTNET_H */
