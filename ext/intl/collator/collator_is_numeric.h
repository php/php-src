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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifndef COLLATOR_IS_NUMERIC_H
#define COLLATOR_IS_NUMERIC_H

#include <php.h>
#include <unicode/uchar.h>
#include <stdint.h>

uint8_t collator_is_numeric( UChar *str, int32_t length, zend_long *lval, double *dval, bool allow_errors );

#endif // COLLATOR_IS_NUMERIC_H
