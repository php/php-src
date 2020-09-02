/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
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
