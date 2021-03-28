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

#ifndef DATETIMEPATTERNGENERATOR_CLASS_H
#define DATETIMEPATTERNGENERATOR_CLASS_H

//redefinition of inline in PHP headers causes problems, so include this before
#include <math.h>

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
	intl_error  err;

	// ICU break iterator
	DateTimePatternGenerator*	dtpg;

	zend_object	zo;
} IntlDateTimePatternGenerator_object;

static inline IntlDateTimePatternGenerator_object *php_intl_datetimepatterngenerator_fetch_object(zend_object *obj) {
	return (IntlDateTimePatternGenerator_object *)((char*)(obj) - XtOffsetOf(IntlDateTimePatternGenerator_object, zo));
}
#define Z_INTL_DATETIMEPATTERNGENERATOR_P(zv) php_intl_datetimepatterngenerator_fetch_object(Z_OBJ_P(zv))

#define DTPATTERNGEN_ERROR(dtpgo)		(dtpgo)->err
#define DTPATTERNGEN_ERROR_P(dtpgo)		&(DTPATTERNGEN_ERROR(dtpgo))

#define DTPATTERNGEN_ERROR_CODE(dtpgo)	INTL_ERROR_CODE(DTPATTERNGEN_ERROR(dtpgo))
#define DTPATTERNGEN_ERROR_CODE_P(dtpgo)	&(INTL_ERROR_CODE(DTPATTERNGEN_ERROR(dtpgo)))

#define DTPATTERNGEN_METHOD_INIT_VARS		        INTL_METHOD_INIT_VARS(IntlDateTimePatternGenerator, dtpgo)
#define DTPATTERNGEN_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(INTL_DATETIMEPATTERNGENERATOR, dtpgo)
#define DTPATTERNGEN_METHOD_FETCH_OBJECT \
	DTPATTERNGEN_METHOD_FETCH_OBJECT_NO_CHECK; \
	if (dtpgo->dtpg == NULL) \
	{ \
		zend_throw_error(NULL, "Found unconstructed IntlDateTimePatternGenerator"); \
		RETURN_THROWS(); \
	}

void dateformat_register_IntlDateTimePatternGenerator_class(void);

extern zend_class_entry *IntlDateTimePatternGenerator_ce_ptr;

extern zend_object_handlers IntlDateTimePatternGenerator_handlers;

#endif /* #ifndef DATETIMEPATTERNGENERATOR_CLASS_H */
