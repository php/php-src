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
  | Author: Artem Ukrainskiy <art.ukrainskiy@gmail.com>                    |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_STRMATCH_H
#define PHP_STRMATCH_H

#include "php.h"

extern zend_module_entry strmatch_module_entry;
#define phpext_strmatch_ptr &strmatch_module_entry

#define PHP_STRMATCH_VERSION "0.9-dev"

#endif /* PHP_STRMATCH_H */
