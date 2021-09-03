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
  | Author: John Coggeshall <john@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_TIDY_H
#define PHP_TIDY_H

extern zend_module_entry tidy_module_entry;
#define phpext_tidy_ptr &tidy_module_entry

#include "php_version.h"
#define PHP_TIDY_VERSION PHP_VERSION

ZEND_BEGIN_MODULE_GLOBALS(tidy)
	char *default_config;
	bool clean_output;
ZEND_END_MODULE_GLOBALS(tidy)

#define TG(v) ZEND_MODULE_GLOBALS_ACCESSOR(tidy, v)

#if defined(ZTS) && defined(COMPILE_DL_TIDY)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif
