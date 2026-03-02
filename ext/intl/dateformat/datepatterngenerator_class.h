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
   | Authors: Mel Dafert (mel@dafert.at)                                  |
   +----------------------------------------------------------------------+
*/

#ifndef DATEPATTERNGENERATOR_CLASS_H
#define DATEPATTERNGENERATOR_CLASS_H

#include <php.h>
#include "intl_common.h"
#include "intl_error.h"
#include "intl_data.h"

#ifndef USE_DATETIMEPATTERNGENERATOR_POINTER
typedef void DateTimePatternGenerator;
#else
using icu::DateTimePatternGenerator;
#endif

typedef struct {
	// 	error handling
	intl_error err;

	// ICU break iterator
	DateTimePatternGenerator *dtpg;

	zend_object	zo;
} IntlDatePatternGenerator_object;

static inline IntlDatePatternGenerator_object *php_intl_datepatterngenerator_fetch_object(zend_object *obj) {
	return (IntlDatePatternGenerator_object *)((char*)(obj) - XtOffsetOf(IntlDatePatternGenerator_object, zo));
}
#define Z_INTL_DATEPATTERNGENERATOR_P(zv) php_intl_datepatterngenerator_fetch_object(Z_OBJ_P(zv))

#define DTPATTERNGEN_ERROR(dtpgo)		(dtpgo)->err
#define DTPATTERNGEN_ERROR_P(dtpgo)		&(DTPATTERNGEN_ERROR(dtpgo))

#define DTPATTERNGEN_ERROR_CODE(dtpgo)	INTL_ERROR_CODE(DTPATTERNGEN_ERROR(dtpgo))
#define DTPATTERNGEN_ERROR_CODE_P(dtpgo)	&(INTL_ERROR_CODE(DTPATTERNGEN_ERROR(dtpgo)))

#define DTPATTERNGEN_METHOD_INIT_VARS		        INTL_METHOD_INIT_VARS(IntlDatePatternGenerator, dtpgo)
#define DTPATTERNGEN_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(INTL_DATEPATTERNGENERATOR, dtpgo)
#define DTPATTERNGEN_METHOD_FETCH_OBJECT \
	DTPATTERNGEN_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (dtpgo->dtpg == NULL) \
	{ \
		zend_throw_error(NULL, "Found unconstructed IntlDatePatternGenerator"); \
		RETURN_THROWS(); \
	}

void dateformat_register_IntlDatePatternGenerator_class(void);

extern zend_class_entry *IntlDatePatternGenerator_ce_ptr;

extern zend_object_handlers IntlDatePatternGenerator_handlers;

#endif /* #ifndef DATEPATTERNGENERATOR_CLASS_H */
