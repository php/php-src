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
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_MYSQLI_H
#define PHP_MYSQLI_H

#define phpext_mysqli_ptr &mysqli_module_entry
extern zend_module_entry mysqli_module_entry;

#include "php_version.h"
#define PHP_MYSQLI_VERSION PHP_VERSION

#endif	/* PHP_MYSQLI.H */
