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
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_VERSIONING_H
#define PHP_VERSIONING_H

#include "ext/standard/basic_functions.h"

PHPAPI char *php_canonicalize_version(const char *);
PHPAPI int php_version_compare(const char *, const char *);

#endif
