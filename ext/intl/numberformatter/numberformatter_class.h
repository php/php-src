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
   | Authors: David Carlier <devnexen@gmail.com>                          |
   +----------------------------------------------------------------------+
*/

#ifndef NUMBERFORMATTER_CLASS_H
#define NUMBERFORMATTER_CLASS_H

#include <php.h>
#include "intl_common.h"
#include "intl_error.h"
#include "intl_data.h"

#ifdef __cplusplus
#include <unicode/numberformatter.h>
using icu::number::LocalizedNumberFormatter;
#else
typedef void LocalizedNumberFormatter;
#endif

typedef struct {
	intl_error                error;
	LocalizedNumberFormatter *unumf;
} numberformatter_data;

typedef struct {
	numberformatter_data nf_data;
	zend_object          zo;
} IntlNumberFormatter_object;

#define php_intl_numberformatter_fetch_object(obj) ZEND_CONTAINER_OF(obj, IntlNumberFormatter_object, zo)
#define Z_INTL_INTLNUMBERFORMATTER_P(zv) php_intl_numberformatter_fetch_object(Z_OBJ_P(zv))

#define NUMBERFORMATTER_OBJECT(nfo) (nfo)->nf_data.unumf

BEGIN_EXTERN_C()
void numberformatter_register_class(void);
extern zend_class_entry *IntlNumberFormatter_ce_ptr;
END_EXTERN_C()

#endif
