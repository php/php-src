/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: John Coggeshall <john@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_TIDY_H
#define PHP_TIDY_H

extern zend_module_entry tidy_module_entry;
#define phpext_tidy_ptr &tidy_module_entry

#include "php_version.h"
#define PHP_TIDY_VERSION PHP_VERSION

#define TIDY_METHOD_MAP(name, func_name, arg_types) \
	ZEND_NAMED_FE(name, ZEND_FN(func_name), arg_types)
#define TIDY_NODE_METHOD(name)    PHP_FUNCTION(tnm_ ##name)
#define TIDY_NODE_ME(name, param) TIDY_METHOD_MAP(name, tnm_ ##name, param)
#define TIDY_NODE_PRIVATE_ME(name, param) ZEND_NAMED_ME(name, ZEND_FN(tnm_ ##name), param, ZEND_ACC_PRIVATE)
#define TIDY_DOC_METHOD(name)     PHP_FUNCTION(tdm_ ##name)
#define TIDY_DOC_ME(name, param)  TIDY_METHOD_MAP(name, tdm_ ##name, param)
#define TIDY_ATTR_METHOD(name)    PHP_FUNCTION(tam_ ##name)
#define TIDY_ATTR_ME(name, param) TIDY_METHOD_MAP(name, tam_ ##name, param)

ZEND_BEGIN_MODULE_GLOBALS(tidy)
	char *default_config;
	zend_bool clean_output;
ZEND_END_MODULE_GLOBALS(tidy)

#define TG(v) ZEND_MODULE_GLOBALS_ACCESSOR(tidy, v)

#if defined(ZTS) && defined(COMPILE_DL_TIDY)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
