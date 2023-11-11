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
#define INTLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(INTL_ITERATOR, ii)
#define INTLITERATOR_METHOD_FETCH_OBJECT\
	object = ZEND_THIS; \
	INTLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (ii->iterator == NULL) { \
		zend_throw_error(NULL, "Found unconstructed IntlIterator"); \
		RETURN_THROWS(); \
	}

typedef struct {
	intl_error				err;
	zend_object_iterator	*iterator;
	zend_object				zo;
} IntlIterator_object;


static inline IntlIterator_object *php_intl_iterator_fetch_object(zend_object *obj) {
	return (IntlIterator_object *)((char*)(obj) - XtOffsetOf(IntlIterator_object, zo));
}
#define Z_INTL_ITERATOR_P(zv) php_intl_iterator_fetch_object(Z_OBJ_P(zv))

typedef struct {
	zend_object_iterator	zoi;
	zval					current;
	zval					wrapping_obj;
	void					(*destroy_it)(zend_object_iterator	*iterator);
} zoi_with_current;

extern zend_class_entry *IntlIterator_ce_ptr;
extern zend_object_handlers IntlIterator_handlers;

U_CFUNC void zoi_with_current_dtor(zend_object_iterator *iter);
U_CFUNC int zoi_with_current_valid(zend_object_iterator *iter);
U_CFUNC zval *zoi_with_current_get_current_data(zend_object_iterator *iter);
U_CFUNC void zoi_with_current_invalidate_current(zend_object_iterator *iter);

#ifdef __cplusplus
using icu::StringEnumeration;
U_CFUNC void IntlIterator_from_StringEnumeration(StringEnumeration *se, zval *object);
#endif

U_CFUNC void intl_register_common_symbols(int module_number);

#endif // INTL_COMMON_ENUM_H
