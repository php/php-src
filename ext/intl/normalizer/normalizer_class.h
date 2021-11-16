/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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

void normalizer_register_Normalizer_class( void );
extern zend_class_entry *Normalizer_ce_ptr;
#endif // #ifndef NORMALIZER_CLASS_H
