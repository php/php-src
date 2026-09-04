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
   | Authors: Weilin Du <weilindu@php.net>                                |
   +----------------------------------------------------------------------+
 */

#ifndef RELDATEFORMATTER_CLASS_H
#define RELDATEFORMATTER_CLASS_H

#include <php.h>
#include <unicode/ureldatefmt.h>

#include "../intl_error.h"

typedef struct {
	intl_error error;
	URelativeDateTimeFormatter *formatter;
	zend_object zo;
} IntlRelativeDateTimeFormatter_object;

#define php_intl_reldateformatter_fetch_object(obj) \
	ZEND_CONTAINER_OF(obj, IntlRelativeDateTimeFormatter_object, zo)
#define Z_INTL_RELDATEFORMATTER_P(zv) \
	php_intl_reldateformatter_fetch_object(Z_OBJ_P(zv))

#define RELDATEFORMATTER_ERROR(obj) ((obj)->error)
#define RELDATEFORMATTER_ERROR_P(obj) (&RELDATEFORMATTER_ERROR(obj))
#define RELDATEFORMATTER_OBJECT(obj) ((obj)->formatter)

#ifdef __cplusplus
extern "C" {
#endif

void reldateformatter_register_class(void);
extern zend_class_entry *IntlRelativeDateTimeFormatter_ce_ptr;

#ifdef __cplusplus
}
#endif

#endif /* RELDATEFORMATTER_CLASS_H */
