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
   | Authors: Bogdan Ungureanu <bogdanungureanu21@gmail.com>              |
   +----------------------------------------------------------------------+
*/

#ifndef RANGEFORMATTER_CLASS_H
#define RANGEFORMATTER_CLASS_H

#include <unicode/numberrangeformatter.h>

#ifdef __cplusplus
using icu::number::LocalizedNumberRangeFormatter;
#else
typedef void LocalizedNumberRangeFormatter;
#endif

typedef struct {
    // error handling
    intl_error      error;

    // formatter handling
    LocalizedNumberRangeFormatter*  unumrf;
} rangeformatter_data;

typedef struct {
    rangeformatter_data  nrf_data;
    zend_object     zo;
} IntlNumberRangeFormatter_object;

static inline IntlNumberRangeFormatter_object *php_intl_numberrangeformatter_fetch_object(zend_object *obj) {
    return (IntlNumberRangeFormatter_object *)((char*)(obj) - XtOffsetOf(IntlNumberRangeFormatter_object, zo));
}

#define Z_INTL_RANGEFORMATTER_P(zv) php_intl_numberrangeformatter_fetch_object(Z_OBJ_P(zv))

#define RANGEFORMATTER_ERROR(nfo) (nfo)->nrf_data.error
#define RANGEFORMATTER_ERROR_P(nfo) &(RANGEFORMATTER_ERROR(nfo))

#define RANGEFORMATTER_OBJECT(nfo) (nfo)->nrf_data.unumrf


void rangeformatter_register_class(void);

#endif
