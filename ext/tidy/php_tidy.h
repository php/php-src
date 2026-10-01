/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
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
