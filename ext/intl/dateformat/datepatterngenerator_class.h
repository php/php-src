/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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
