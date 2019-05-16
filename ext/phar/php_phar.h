/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
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
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PHAR_H
#define PHP_PHAR_H

#define PHP_PHAR_VERSION PHP_VERSION

#include "ext/standard/basic_functions.h"
extern zend_module_entry phar_module_entry;
#define phpext_phar_ptr &phar_module_entry

#ifdef PHP_WIN32
#define PHP_PHAR_API __declspec(dllexport)
#else
#define PHP_PHAR_API PHPAPI
#endif

PHP_PHAR_API int phar_resolve_alias(char *alias, size_t alias_len, char **filename, size_t *filename_len);

#endif /* PHP_PHAR_H */
