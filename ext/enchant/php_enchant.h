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
  | Author: Pierre-Alain Joye <paj@pearfr.org>                           |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_ENCHANT_H
#define PHP_ENCHANT_H

extern zend_module_entry enchant_module_entry;
#define phpext_enchant_ptr &enchant_module_entry

#define PHP_ENCHANT_VERSION PHP_VERSION

#ifdef PHP_WIN32
#define PHP_ENCHANT_API __declspec(dllexport)
#else
#define PHP_ENCHANT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(enchant);
PHP_MSHUTDOWN_FUNCTION(enchant);
PHP_MINFO_FUNCTION(enchant);

#endif	/* PHP_ENCHANT_H */
