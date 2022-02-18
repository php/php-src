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

#include "../intl_cppshims.h"

#include <unicode/dtptngen.h>

#include "../intl_convertcpp.h"

extern "C" {
#include "php_intl.h"
#define USE_DATETIMEPATTERNGENERATOR_POINTER 1
#include "datepatterngenerator_class.h"
#include <zend_exceptions.h>
#include <assert.h>
}

using icu::DateTimePatternGenerator;
using icu::Locale;
using icu::StringPiece;

static zend_result dtpg_ctor(INTERNAL_FUNCTION_PARAMETERS, zend_error_handling *error_handling, bool *error_handling_replaced)
{
	char *locale_str;
	size_t locale_len = 0;
	IntlDatePatternGenerator_object* dtpgo;

	intl_error_reset(NULL);
	zval *object = return_value;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(locale_str, locale_len)
	ZEND_PARSE_PARAMETERS_END_EX(return FAILURE);

	if (error_handling != NULL) {
		zend_replace_error_handling(EH_THROW, IntlException_ce_ptr, error_handling);
		*error_handling_replaced = 1;
	}

	DTPATTERNGEN_METHOD_FETCH_OBJECT_NO_CHECK;

	if (dtpgo->dtpg != NULL) {
		intl_errors_set(DTPATTERNGEN_ERROR_P(dtpgo), U_ILLEGAL_ARGUMENT_ERROR, "Cannot call constructor twice", 0);
		return FAILURE;
	}

	INTL_CHECK_LOCALE_LEN_OR_FAILURE(locale_len);
	if (locale_len == 0) {
		locale_str = (char *) intl_locale_get_default();
	}
	Locale locale = Locale::createFromName(locale_str);

	dtpgo->dtpg = DateTimePatternGenerator::createInstance(
		locale,
		DTPATTERNGEN_ERROR_CODE(dtpgo));

	if (U_FAILURE(DTPATTERNGEN_ERROR_CODE(dtpgo))) {
		intl_error_set(NULL, DTPATTERNGEN_ERROR_CODE(dtpgo),
				"Error creating DateTimePatternGenerator",
				0);
		return FAILURE;
	}

	return SUCCESS;
}

U_CFUNC PHP_METHOD( IntlDatePatternGenerator, create )
{
    object_init_ex( return_value, IntlDatePatternGenerator_ce_ptr );
    if (dtpg_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, NULL, NULL) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_NULL();
	}
}

U_CFUNC PHP_METHOD( IntlDatePatternGenerator, __construct )
{
	zend_error_handling error_handling;
	bool error_handling_replaced = 0;

	/* return_value param is being changed, therefore we will always return
	 * NULL here */
	return_value = ZEND_THIS;
	if (dtpg_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, &error_handling, &error_handling_replaced) == FAILURE) {
		if (!EG(exception)) {
			zend_string *err = intl_error_get_message(NULL);
			zend_throw_exception(IntlException_ce_ptr, ZSTR_VAL(err), intl_error_get_code(NULL));
			zend_string_release_ex(err, 0);
		}
	}
	if (error_handling_replaced) {
		zend_restore_error_handling(&error_handling);
	}
}


U_CFUNC PHP_METHOD( IntlDatePatternGenerator, getBestPattern )
{
	char			*skeleton_str	= NULL;
	size_t			skeleton_len;
	UnicodeString	skeleton_uncleaned;

	DTPATTERNGEN_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Os",
		&object, IntlDatePatternGenerator_ce_ptr,  &skeleton_str, &skeleton_len ) == FAILURE )
	{
		RETURN_THROWS();
	}

	DTPATTERNGEN_METHOD_FETCH_OBJECT;

	intl_stringFromChar(skeleton_uncleaned, skeleton_str, skeleton_len, DTPATTERNGEN_ERROR_CODE_P(dtpgo));

	INTL_METHOD_CHECK_STATUS(dtpgo, "Skeleton is not a valid UTF-8 string");

	UnicodeString skeleton = dtpgo->dtpg->getSkeleton(skeleton_uncleaned, DTPATTERNGEN_ERROR_CODE(dtpgo));

	INTL_METHOD_CHECK_STATUS(dtpgo, "Error getting cleaned skeleton");

	UnicodeString result = dtpgo->dtpg->getBestPattern(skeleton, DTPATTERNGEN_ERROR_CODE(dtpgo));

	INTL_METHOD_CHECK_STATUS(dtpgo, "Error retrieving pattern");

	zend_string *u8str = intl_charFromString(result, DTPATTERNGEN_ERROR_CODE_P(dtpgo));

	INTL_METHOD_CHECK_STATUS(dtpgo, "Error converting result to UTF-8");

	RETVAL_STR(u8str);
}
