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
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef NORMALIZER_CLASS_H
#define NORMALIZER_CLASS_H

#include <php.h>

#include "intl_common.h"
#include "intl_error.h"

#include <unicode/unorm.h>

typedef struct {
	zend_object     zo;

	// error value not used currently
	intl_error  err;

} Normalizer_object;

#define NORMALIZER_ERROR(co) (co)->err
#define NORMALIZER_ERROR_P(co) &(NORMALIZER_ERROR(co))

#define NORMALIZER_ERROR_CODE(co)   INTL_ERROR_CODE(NORMALIZER_ERROR(co))
#define NORMALIZER_ERROR_CODE_P(co) &(INTL_ERROR_CODE(NORMALIZER_ERROR(co)))

#ifdef __cplusplus
extern "C" {
#endif
void normalizer_register_Normalizer_class( void );
#ifdef __cplusplus
}
#endif
extern zend_class_entry *Normalizer_ce_ptr;
#endif // #ifndef NORMALIZER_CLASS_H
