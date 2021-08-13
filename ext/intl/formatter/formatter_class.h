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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef FORMATTER_CLASS_H
#define FORMATTER_CLASS_H

#include <php.h>

#include "intl_common.h"
#include "intl_error.h"
#include "intl_data.h"
#include "formatter_data.h"

typedef struct {
	formatter_data  nf_data;
	zend_object     zo;
} NumberFormatter_object;

static inline NumberFormatter_object *php_intl_number_format_fetch_object(zend_object *obj) {
	return (NumberFormatter_object *)((char*)(obj) - XtOffsetOf(NumberFormatter_object, zo));
}
#define Z_INTL_NUMBERFORMATTER_P(zv) php_intl_number_format_fetch_object(Z_OBJ_P(zv))

void formatter_register_class( void );
extern zend_class_entry *NumberFormatter_ce_ptr;

/* Auxiliary macros */

#define FORMATTER_METHOD_INIT_VARS				INTL_METHOD_INIT_VARS(NumberFormatter, nfo)
#define FORMATTER_OBJECT(nfo)					(nfo)->nf_data.unum
#define FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(INTL_NUMBERFORMATTER, nfo)
#define FORMATTER_METHOD_FETCH_OBJECT \
	FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (FORMATTER_OBJECT(nfo) == NULL) \
	{ \
		zend_throw_error(NULL, "Found unconstructed NumberFormatter"); \
		RETURN_THROWS(); \
	}


#endif // #ifndef FORMATTER_CLASS_H
