/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifndef INTL_COMMON_ENUM_H
#define INTL_COMMON_ENUM_H

#include <unicode/umachine.h>
#ifdef __cplusplus
#include <unicode/strenum.h>
extern "C" {
#include <math.h>
#endif
#include <php.h>
#include "../intl_error.h"
#include "../intl_data.h"
#ifdef __cplusplus
}
#endif

#define INTLITERATOR_ERROR(ii)						(ii)->err
#define INTLITERATOR_ERROR_P(ii)					&(INTLITERATOR_ERROR(ii))

#define INTLITERATOR_ERROR_CODE(ii)					INTL_ERROR_CODE(INTLITERATOR_ERROR(ii))
#define INTLITERATOR_ERROR_CODE_P(ii)				&(INTL_ERROR_CODE(INTLITERATOR_ERROR(ii)))

#define INTLITERATOR_METHOD_INIT_VARS				INTL_METHOD_INIT_VARS(IntlIterator, ii)
#define INTLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(IntlIterator, ii)
#define INTLITERATOR_METHOD_FETCH_OBJECT\
	object = getThis(); \
	INTLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (ii->iterator == NULL) { \
		intl_errors_set(&ii->err, U_ILLEGAL_ARGUMENT_ERROR, "Found unconstructed IntlIterator", 0 TSRMLS_CC); \
		RETURN_FALSE; \
	}

typedef struct {
	zend_object				zo;
	intl_error				err;
	zend_object_iterator	*iterator;
} IntlIterator_object;

typedef struct {
	zend_object_iterator	zoi;
	zval					*current;
	zval					*wrapping_obj;
	void					(*destroy_it)(zend_object_iterator	*iterator TSRMLS_DC);
} zoi_with_current;

extern zend_class_entry *IntlIterator_ce_ptr;
extern zend_object_handlers IntlIterator_handlers;

U_CFUNC void zoi_with_current_dtor(zend_object_iterator *iter TSRMLS_DC);
U_CFUNC int zoi_with_current_valid(zend_object_iterator *iter TSRMLS_DC);
U_CFUNC void zoi_with_current_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
U_CFUNC void zoi_with_current_invalidate_current(zend_object_iterator *iter TSRMLS_DC);

#ifdef __cplusplus
U_CFUNC void IntlIterator_from_StringEnumeration(StringEnumeration *se, zval *object TSRMLS_DC);
#endif

U_CFUNC void intl_register_IntlIterator_class(TSRMLS_D);

#endif // INTL_COMMON_ENUM_H
