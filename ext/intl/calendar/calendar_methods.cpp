/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "../intl_cppshims.h"

#include <unicode/locid.h>
#include <unicode/calendar.h>
#include <unicode/ustring.h>

#include "../intl_convertcpp.h"
#include "../common/common_date.h"

extern "C" {
#include "../php_intl.h"
#define USE_TIMEZONE_POINTER 1
#include "../timezone/timezone_class.h"
#define USE_CALENDAR_POINTER 1
#include "calendar_class.h"
#include "../intl_convert.h"
#include <zend_exceptions.h>
#include <zend_interfaces.h>
#include <ext/date/php_date.h>
}
#include "../common/common_enum.h"

using icu::Locale;

U_CFUNC PHP_METHOD(IntlCalendar, __construct)
{
	zend_throw_exception( NULL,
		"An object of this type cannot be created with the new operator",
		0 );
}

U_CFUNC PHP_FUNCTION(intlcal_create_instance)
{
	zval		*zv_timezone	= NULL;
	const char	*locale_str		= NULL;
	size_t			dummy;
	TimeZone	*timeZone;
	UErrorCode	status			= U_ZERO_ERROR;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zs!",
			&zv_timezone, &locale_str, &dummy) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_create_calendar: bad arguments", 0);
		RETURN_NULL();
	}

	timeZone = timezone_process_timezone_argument(zv_timezone, NULL,
		"intlcal_create_instance");
	if (timeZone == NULL) {
		RETURN_NULL();
	}

	if (!locale_str) {
		locale_str = intl_locale_get_default();
	}

	Calendar *cal = Calendar::createInstance(timeZone,
		Locale::createFromName(locale_str), status);
	if (cal == NULL) {
		delete timeZone;
		intl_error_set(NULL, status, "Error creating ICU Calendar object", 0);
		RETURN_NULL();
	}

	calendar_object_create(return_value, cal);
}

class BugStringCharEnumeration : public StringEnumeration
{
public:
	explicit BugStringCharEnumeration(UEnumeration* _uenum) : uenum(_uenum) {}

	~BugStringCharEnumeration()
	{
		uenum_close(uenum);
	}

	int32_t count(UErrorCode& status) const {
		return uenum_count(uenum, &status);
	}

	virtual const UnicodeString* snext(UErrorCode& status)
	{
		int32_t length;
		const UChar* str = uenum_unext(uenum, &length, &status);
		if (str == 0 || U_FAILURE(status)) {
			return 0;
		}
		return &unistr.setTo(str, length);
	}

	virtual const char* next(int32_t *resultLength, UErrorCode &status)
	{
		int32_t length = -1;
		const char* str = uenum_next(uenum, &length, &status);
		if (str == 0 || U_FAILURE(status)) {
			return 0;
		}
		if (resultLength) {
			//the bug is that uenum_next doesn't set the length
			*resultLength = (length == -1) ? (int32_t)strlen(str) : length;
		}

		return str;
	}

	void reset(UErrorCode& status)
	{
		uenum_reset(uenum, &status);
	}

	virtual UClassID getDynamicClassID() const;

	static UClassID U_EXPORT2 getStaticClassID();

private:
	UEnumeration *uenum;
};
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(BugStringCharEnumeration)

U_CFUNC PHP_FUNCTION(intlcal_get_keyword_values_for_locale)
{
	UErrorCode	status = U_ZERO_ERROR;
	char		*key,
				*locale;
	size_t			key_len,
				locale_len;
	zend_bool	commonly_used;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssb",
			&key, &key_len, &locale, &locale_len, &commonly_used) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_keyword_values_for_locale: bad arguments", 0);
		RETURN_FALSE;
	}

	//does not work; see ICU bug 9194
#if 0
	StringEnumeration *se = Calendar::getKeywordValuesForLocale(key,
		Locale::createFromName(locale), (UBool)commonly_used,
		status);
	if (se == NULL) {
		intl_error_set(NULL, status, "intlcal_get_keyword_values_for_locale: "
			"error calling underlying method", 0);
		RETURN_FALSE;
	}
#else
    UEnumeration *uenum = ucal_getKeywordValuesForLocale(
		key, locale, !!commonly_used, &status);
    if (U_FAILURE(status)) {
        uenum_close(uenum);
		intl_error_set(NULL, status, "intlcal_get_keyword_values_for_locale: "
			"error calling underlying method", 0);
        RETURN_FALSE;
    }

    StringEnumeration *se = new BugStringCharEnumeration(uenum);
#endif

	IntlIterator_from_StringEnumeration(se, return_value);
}

U_CFUNC PHP_FUNCTION(intlcal_get_now)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_now: bad arguments", 0);
		RETURN_FALSE;
	}

	RETURN_DOUBLE((double)Calendar::getNow());
}

U_CFUNC PHP_FUNCTION(intlcal_get_available_locales)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_available_locales: bad arguments", 0);
		RETURN_FALSE;
	}

	int32_t count;
	const Locale *availLocales = Calendar::getAvailableLocales(count);
	array_init(return_value);
	for (int i = 0; i < count; i++) {
		Locale locale = availLocales[i];
		add_next_index_string(return_value, locale.getName());
	}
}

static void _php_intlcal_field_uec_ret_in32t_method(
		int32_t (Calendar::*func)(UCalendarDateFields, UErrorCode&) const,
		const char *method_name,
		INTERNAL_FUNCTION_PARAMETERS)
{
	zend_long	field;
	char	*message;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &field) == FAILURE) {
		spprintf(&message, 0, "%s: bad arguments", method_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, message, 1);
		efree(message);
		RETURN_FALSE;
	}

	if (field < 0 || field >= UCAL_FIELD_COUNT) {
		spprintf(&message, 0, "%s: invalid field", method_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, message, 1);
		efree(message);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t result = (co->ucal->*func)(
		(UCalendarDateFields)field, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get)
{
	_php_intlcal_field_uec_ret_in32t_method(&Calendar::get,
		"intlcal_get", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_time)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_time: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	UDate result = co->ucal->getTime(CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co,
		"intlcal_get_time: error calling ICU Calendar::getTime");

	RETURN_DOUBLE((double)result);
}

U_CFUNC PHP_FUNCTION(intlcal_set_time)
{
	double	time_arg;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Od",
			&object, Calendar_ce_ptr, &time_arg) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_time: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->setTime((UDate)time_arg, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "Call to underlying method failed");

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_add)
{
	zend_long	field,
			amount;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Oll", &object, Calendar_ce_ptr, &field, &amount) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_add: bad arguments", 0);
		RETURN_FALSE;
	}

	if (field < 0 || field >= UCAL_FIELD_COUNT) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_add: invalid field", 0);
		RETURN_FALSE;
	}
	if (amount < INT32_MIN || amount > INT32_MAX) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_add: amount out of bounds", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->add((UCalendarDateFields)field, (int32_t)amount, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "intlcal_add: Call to underlying method failed");

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_set_time_zone)
{
	zval			*zv_timezone;
	TimeZone		*timeZone;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Oz!", &object, Calendar_ce_ptr, &zv_timezone) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_time_zone: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	if (zv_timezone == NULL) {
		RETURN_TRUE; /* the method does nothing if passed null */
	}

	timeZone = timezone_process_timezone_argument(zv_timezone,
			CALENDAR_ERROR_P(co), "intlcal_set_time_zone");
	if (timeZone == NULL) {
		RETURN_FALSE;
	}

	co->ucal->adoptTimeZone(timeZone);

	RETURN_TRUE;
}


static void _php_intlcal_before_after(
		UBool (Calendar::*func)(const Calendar&, UErrorCode&) const,
		INTERNAL_FUNCTION_PARAMETERS)
{
	zval			*when_object;
	Calendar_object	*when_co;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"OO", &object, Calendar_ce_ptr, &when_object, Calendar_ce_ptr)
			== FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_before/after: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	when_co = Z_INTL_CALENDAR_P(when_object);
	if (when_co->ucal == NULL) {
		intl_errors_set(&co->err, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_before/after: Other IntlCalendar was unconstructed", 0);
		RETURN_FALSE;
	}

	UBool res = (co->ucal->*func)(*when_co->ucal, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "intlcal_before/after: Error calling ICU method");

	RETURN_BOOL((int)res);
}

U_CFUNC PHP_FUNCTION(intlcal_after)
{
	_php_intlcal_before_after(&Calendar::after, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_before)
{
	_php_intlcal_before_after(&Calendar::before, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_set)
{
	zend_long	arg1, arg2, arg3, arg4, arg5, arg6;
	zval	args_a[7] = {0},
			*args = args_a;
	int		i;
	int		variant; /* number of args of the set() overload */
	CALENDAR_METHOD_INIT_VARS;

	object = getThis();

	/* must come before zpp because zpp would convert the args in the stack to 0 */
	if (ZEND_NUM_ARGS() > (object ? 6 : 7) ||
				zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set: too many arguments", 0);
		RETURN_FALSE;
	}
	if (!object) {
		args++;
	}
	variant = ZEND_NUM_ARGS() - (object ? 0 : 1);
	while (variant > 2 && Z_TYPE(args[variant - 1]) == IS_NULL) {
		variant--;
	}

	if (variant == 4 ||
			zend_parse_method_parameters(ZEND_NUM_ARGS(), object,
			"Oll|llll",	&object, Calendar_ce_ptr, &arg1, &arg2, &arg3, &arg4,
			&arg5, &arg6) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set: bad arguments", 0);
		RETURN_FALSE;
	}

	for (i = 0; i < variant; i++) {
		if (Z_LVAL(args[i]) < INT32_MIN || Z_LVAL(args[i]) > INT32_MAX) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intlcal_set: at least one of the arguments has an absolute "
				"value that is too large", 0);
			RETURN_FALSE;
		}
	}

	if (variant == 2 && (arg1 < 0 || arg1 >= UCAL_FIELD_COUNT)) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set: invalid field", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	if (variant == 2) {
		co->ucal->set((UCalendarDateFields)arg1, (int32_t)arg2);
	} else if (variant == 3) {
		co->ucal->set((int32_t)arg1, (int32_t)arg2, (int32_t)arg3);
	} else if (variant == 5) {
		co->ucal->set((int32_t)arg1, (int32_t)arg2, (int32_t)arg3, (int32_t)arg4, (int32_t)arg5);
	} else if (variant == 6) {
		co->ucal->set((int32_t)arg1, (int32_t)arg2, (int32_t)arg3, (int32_t)arg4, (int32_t)arg5, (int32_t)arg6);
	}

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_roll)
{
	zend_long		field,
				value;
	zval		args_a[3]		 = {0},
				*args			 = args_a;
	zend_bool	bool_variant_val = (zend_bool)-1;
	CALENDAR_METHOD_INIT_VARS;

	object = getThis();

	if (ZEND_NUM_ARGS() > (object ? 2 :3) ||
			zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set: too many arguments", 0);
		RETURN_FALSE;
	}
	if (!object) {
		args++;
	}
	if (!Z_ISUNDEF(args[1]) && (Z_TYPE(args[1]) == IS_TRUE || Z_TYPE(args[1]) == IS_FALSE)) {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS(), object,
				"Olb", &object, Calendar_ce_ptr, &field, &bool_variant_val)
				== FAILURE) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intlcal_roll: bad arguments", 0);
			RETURN_FALSE;
		}
		bool_variant_val = Z_TYPE(args[1]) == IS_TRUE? 1 : 0;
	} else if (zend_parse_method_parameters(ZEND_NUM_ARGS(), object,
			"Oll", &object, Calendar_ce_ptr, &field, &value) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_roll: bad arguments", 0);
		RETURN_FALSE;
	}

	if (field < 0 || field >= UCAL_FIELD_COUNT) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_roll: invalid field", 0);
		RETURN_FALSE;
	}
	if (bool_variant_val == (zend_bool)-1 &&
			(value < INT32_MIN || value > INT32_MAX)) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_roll: value out of bounds", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	if (bool_variant_val != (zend_bool)-1) {
		co->ucal->roll((UCalendarDateFields)field, (UBool)bool_variant_val,
			CALENDAR_ERROR_CODE(co));
	} else {
		co->ucal->roll((UCalendarDateFields)field, (int32_t)value,
			CALENDAR_ERROR_CODE(co));
	}
	INTL_METHOD_CHECK_STATUS(co, "intlcal_roll: Error calling ICU Calendar::roll");

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_clear)
{
	zend_long field;
	zend_bool field_is_null = 1;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(),
			getThis(), "O|l!", &object, Calendar_ce_ptr, &field, &field_is_null) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_clear: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	if (field_is_null) {
		co->ucal->clear();
	} else {
		if (field < 0 || field >= UCAL_FIELD_COUNT) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"intlcal_clear: invalid field", 0);
			RETURN_FALSE;
		}

		co->ucal->clear((UCalendarDateFields)field);
	}

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_field_difference)
{
	zend_long	field;
	double	when;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Odl", &object, Calendar_ce_ptr, &when, &field)	== FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_field_difference: bad arguments", 0);
		RETURN_FALSE;
	}

	if (field < 0 || field >= UCAL_FIELD_COUNT) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_field_difference: invalid field", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t result = co->ucal->fieldDifference((UDate)when,
		(UCalendarDateFields)field, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co,
		"intlcal_field_difference: Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get_actual_maximum)
{
	_php_intlcal_field_uec_ret_in32t_method(&Calendar::getActualMaximum,
		"intlcal_get_actual_maximum", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_actual_minimum)
{
	_php_intlcal_field_uec_ret_in32t_method(&Calendar::getActualMinimum,
		"intlcal_get_actual_minimum", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_day_of_week_type)
{
	zend_long	dow;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &dow) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_day_of_week_type: bad arguments", 0);
		RETURN_FALSE;
	}

	if (dow < UCAL_SUNDAY || dow > UCAL_SATURDAY) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_day_of_week_type: invalid day of week", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t result = co->ucal->getDayOfWeekType(
		(UCalendarDaysOfWeek)dow, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co,
		"intlcal_get_day_of_week_type: Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get_first_day_of_week)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_first_day_of_week: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t result = co->ucal->getFirstDayOfWeek(CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co,
		"intlcal_get_first_day_of_week: Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

static void _php_intlcal_field_ret_in32t_method(
		int32_t (Calendar::*func)(UCalendarDateFields) const,
		const char *method_name,
		INTERNAL_FUNCTION_PARAMETERS)
{
	zend_long	field;
	char	*message;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &field) == FAILURE) {
		spprintf(&message, 0, "%s: bad arguments", method_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, message, 1);
		efree(message);
		RETURN_FALSE;
	}

	if (field < 0 || field >= UCAL_FIELD_COUNT) {
		spprintf(&message, 0, "%s: invalid field", method_name);
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, message, 1);
		efree(message);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t result = (co->ucal->*func)((UCalendarDateFields)field);
	INTL_METHOD_CHECK_STATUS(co, "Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get_greatest_minimum)
{
	_php_intlcal_field_ret_in32t_method(&Calendar::getGreatestMinimum,
		"intlcal_get_greatest_minimum", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_least_maximum)
{
	_php_intlcal_field_ret_in32t_method(&Calendar::getLeastMaximum,
		"intlcal_get_least_maximum", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_locale)
{
	zend_long	locale_type;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &locale_type) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_locale: bad arguments", 0);
		RETURN_FALSE;
	}

	if (locale_type != ULOC_ACTUAL_LOCALE && locale_type != ULOC_VALID_LOCALE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_locale: invalid locale type", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	Locale locale = co->ucal->getLocale((ULocDataLocaleType)locale_type,
		CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co,
		"intlcal_get_locale: Call to ICU method has failed");

	RETURN_STRING(locale.getName());
}

U_CFUNC PHP_FUNCTION(intlcal_get_maximum)
{
	_php_intlcal_field_ret_in32t_method(&Calendar::getMaximum,
		"intlcal_get_maximum", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_minimal_days_in_first_week)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_minimal_days_in_first_week: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	uint8_t result = co->ucal->getMinimalDaysInFirstWeek();
	INTL_METHOD_CHECK_STATUS(co,
		"intlcal_get_first_day_of_week: Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get_minimum)
{
	_php_intlcal_field_ret_in32t_method(&Calendar::getMinimum,
		"intlcal_get_minimum", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_time_zone)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_time_zone: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	TimeZone *tz = co->ucal->getTimeZone().clone();
	if (tz == NULL) {
		intl_errors_set(CALENDAR_ERROR_P(co), U_MEMORY_ALLOCATION_ERROR,
			"intlcal_get_time_zone: could not clone TimeZone", 0);
		RETURN_FALSE;
	}

	timezone_object_construct(tz, return_value, 1);
}

U_CFUNC PHP_FUNCTION(intlcal_get_type)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_type: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_STRING(co->ucal->getType());
}

U_CFUNC PHP_FUNCTION(intlcal_get_weekend_transition)
{
	zend_long	dow;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &dow) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_weekend_transition: bad arguments", 0);
		RETURN_FALSE;
	}

	if (dow < UCAL_SUNDAY || dow > UCAL_SATURDAY) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_weekend_transition: invalid day of week", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t res = co->ucal->getWeekendTransition((UCalendarDaysOfWeek)dow,
		CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "intlcal_get_weekend_transition: "
		"Error calling ICU method");

	RETURN_LONG((zend_long)res);
}

U_CFUNC PHP_FUNCTION(intlcal_in_daylight_time)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_in_daylight_time: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	UBool ret = co->ucal->inDaylightTime(CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "intlcal_in_daylight_time: "
		"Error calling ICU method");

	RETURN_BOOL((int)ret);
}

U_CFUNC PHP_FUNCTION(intlcal_is_equivalent_to)
{
	zval			*other_object;
	Calendar_object *other_co;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"OO", &object, Calendar_ce_ptr, &other_object, Calendar_ce_ptr)
			== FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_is_equivalent_to: bad arguments", 0);
		RETURN_FALSE;
	}

	other_co = Z_INTL_CALENDAR_P(other_object);
	if (other_co->ucal == NULL) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "intlcal_is_equivalent_to:"
			" Other IntlCalendar is unconstructed", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_BOOL((int)co->ucal->isEquivalentTo(*other_co->ucal));
}

U_CFUNC PHP_FUNCTION(intlcal_is_lenient)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_is_lenient: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_BOOL((int)co->ucal->isLenient());
}

U_CFUNC PHP_FUNCTION(intlcal_is_set)
{
	zend_long field;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &field) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_is_set: bad arguments", 0);
		RETURN_FALSE;
	}

	if (field < 0 || field >= UCAL_FIELD_COUNT) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_is_set: invalid field", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_BOOL((int)co->ucal->isSet((UCalendarDateFields)field));
}

U_CFUNC PHP_FUNCTION(intlcal_is_weekend)
{
	double date;
	zend_bool date_is_null = 1;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
				"O|d!", &object, Calendar_ce_ptr, &date, &date_is_null) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_is_weekend: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	if (date_is_null) {
		RETURN_BOOL((int)co->ucal->isWeekend());
	} else {
		UBool ret = co->ucal->isWeekend((UDate)date, CALENDAR_ERROR_CODE(co));
		INTL_METHOD_CHECK_STATUS(co, "intlcal_is_weekend: "
			"Error calling ICU method");
		RETURN_BOOL((int)ret);
	}
}


U_CFUNC PHP_FUNCTION(intlcal_set_first_day_of_week)
{
	zend_long	dow;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &dow) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_first_day_of_week: bad arguments", 0);
		RETURN_FALSE;
	}

	if (dow < UCAL_SUNDAY || dow > UCAL_SATURDAY) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_first_day_of_week: invalid day of week", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->setFirstDayOfWeek((UCalendarDaysOfWeek)dow);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_set_lenient)
{
	zend_bool is_lenient;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ob", &object, Calendar_ce_ptr, &is_lenient) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_lenient: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->setLenient((UBool) is_lenient);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_set_minimal_days_in_first_week)
{
	zend_long	num_days;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &num_days) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_minimal_days_in_first_week: bad arguments", 0);
		RETURN_FALSE;
	}

	if (num_days < 1 || num_days > 7) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_minimal_days_in_first_week: invalid number of days; "
			"must be between 1 and 7", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->setMinimalDaysInFirstWeek((uint8_t)num_days);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_equals)
{
	zval			*other_object;
	Calendar_object	*other_co;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"OO", &object, Calendar_ce_ptr, &other_object, Calendar_ce_ptr)
			== FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_equals: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;
	other_co = Z_INTL_CALENDAR_P(other_object);
	if (other_co->ucal == NULL) {
		intl_errors_set(&co->err, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_equals: The second IntlCalendar is unconstructed", 0);
		RETURN_FALSE;
	}

	UBool result = co->ucal->equals(*other_co->ucal, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "intlcal_equals: error calling ICU Calendar::equals");

	RETURN_BOOL((int)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get_repeated_wall_time_option)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_repeated_wall_time_option: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_LONG(co->ucal->getRepeatedWallTimeOption());
}

U_CFUNC PHP_FUNCTION(intlcal_get_skipped_wall_time_option)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_skipped_wall_time_option: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_LONG(co->ucal->getSkippedWallTimeOption());
}

U_CFUNC PHP_FUNCTION(intlcal_set_repeated_wall_time_option)
{
	zend_long	option;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &option) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_repeated_wall_time_option: bad arguments", 0);
		RETURN_FALSE;
	}

	if (option != UCAL_WALLTIME_FIRST && option != UCAL_WALLTIME_LAST) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_repeated_wall_time_option: invalid option", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->setRepeatedWallTimeOption((UCalendarWallTimeOption)option);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_set_skipped_wall_time_option)
{
	zend_long	option;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &option) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_skipped_wall_time_option: bad arguments", 0);
		RETURN_FALSE;
	}

	if (option != UCAL_WALLTIME_FIRST && option != UCAL_WALLTIME_LAST
			&& option != UCAL_WALLTIME_NEXT_VALID) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_set_skipped_wall_time_option: invalid option", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->setSkippedWallTimeOption((UCalendarWallTimeOption)option);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_from_date_time)
{
	zval			*zv_arg,
					zv_tmp,
					*zv_datetime  		= NULL,
					zv_timestamp;
	php_date_obj	*datetime;
	char			*locale_str			= NULL;
	size_t				locale_str_len;
	TimeZone		*timeZone;
	UErrorCode		status				= U_ZERO_ERROR;
	Calendar        *cal;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s!",
			&zv_arg, &locale_str, &locale_str_len) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_from_date_time: bad arguments", 0);
		RETURN_NULL();
	}

	if (!(Z_TYPE_P(zv_arg) == IS_OBJECT && instanceof_function(
			Z_OBJCE_P(zv_arg), php_date_get_date_ce()))) {
		object_init_ex(&zv_tmp, php_date_get_date_ce());
		zend_call_method_with_1_params(&zv_tmp, NULL, &Z_OBJCE(zv_tmp)->constructor, "__construct", NULL, zv_arg);
		zv_datetime = &zv_tmp;
		if (EG(exception)) {
			zend_object_store_ctor_failed(Z_OBJ(zv_tmp));
			goto error;
		}
	} else {
		zv_datetime = zv_arg;
	}

	datetime = Z_PHPDATE_P(zv_datetime);
	if (!datetime->time) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_from_date_time: DateTime object is unconstructed",
			0);
		goto error;
	}

	zend_call_method_with_0_params(zv_datetime, php_date_get_date_ce(), NULL, "gettimestamp", &zv_timestamp);
	if (Z_TYPE(zv_timestamp) != IS_LONG) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_from_date_time: bad DateTime; call to "
			"DateTime::getTimestamp() failed", 0);
		zval_ptr_dtor(&zv_timestamp);
		goto error;
	}

	if (!datetime->time->is_localtime) {
		timeZone = TimeZone::getGMT()->clone();
	} else {
		timeZone = timezone_convert_datetimezone(datetime->time->zone_type,
			datetime, 1, NULL, "intlcal_from_date_time");
		if (timeZone == NULL) {
			goto error;
		}
	}

	if (!locale_str) {
		locale_str = const_cast<char*>(intl_locale_get_default());
	}

	cal = Calendar::createInstance(timeZone,
		Locale::createFromName(locale_str), status);
	if (cal == NULL) {
		delete timeZone;
		intl_error_set(NULL, status, "intlcal_from_date_time: "
				"error creating ICU Calendar object", 0);
		goto error;
	}
	cal->setTime(((UDate)Z_LVAL(zv_timestamp)) * 1000., status);
    if (U_FAILURE(status)) {
		/* time zone was adopted by cal; should not be deleted here */
		delete cal;
		intl_error_set(NULL, status, "intlcal_from_date_time: "
				"error creating ICU Calendar::setTime()", 0);
        goto error;
    }

	calendar_object_create(return_value, cal);

error:
	if (zv_datetime && zv_datetime != zv_arg) {
		zval_ptr_dtor(zv_datetime);
	}
}

U_CFUNC PHP_FUNCTION(intlcal_to_date_time)
{
	zval retval;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_to_date_time: bad arguments", 0);
		RETURN_FALSE;
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	/* There are no exported functions in ext/date to this
	 * in a more native fashion */
	double	date = co->ucal->getTime(CALENDAR_ERROR_CODE(co)) / 1000.;
	int64_t	ts;
	char	ts_str[sizeof("@-9223372036854775808")];
	int		ts_str_len;
	zval	ts_zval, tmp;

	INTL_METHOD_CHECK_STATUS(co, "Call to ICU method has failed");

	if (date > (double)U_INT64_MAX || date < (double)U_INT64_MIN) {
		intl_errors_set(CALENDAR_ERROR_P(co), U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_to_date_time: The calendar date is out of the "
			"range for a 64-bit integer", 0);
		RETURN_FALSE;
	}

	ZVAL_UNDEF(&retval);
	ts = (int64_t)date;

	ts_str_len = slprintf(ts_str, sizeof(ts_str), "@%" PRIi64, ts);
	ZVAL_STRINGL(&ts_zval, ts_str, ts_str_len);

	/* Now get the time zone */
	const TimeZone& tz = co->ucal->getTimeZone();
	zval *timezone_zval = timezone_convert_to_datetimezone(
		&tz, CALENDAR_ERROR_P(co), "intlcal_to_date_time", &tmp);
	if (timezone_zval == NULL) {
		zval_ptr_dtor(&ts_zval);
		RETURN_FALSE;
	}

	/* resources allocated from now on */

	/* Finally, instantiate object and call constructor */
	object_init_ex(return_value, php_date_get_date_ce());
	zend_call_method_with_2_params(return_value, NULL, &Z_OBJCE_P(return_value)->constructor, "__construct", NULL, &ts_zval, timezone_zval);
	if (EG(exception)) {
		intl_errors_set(CALENDAR_ERROR_P(co), U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_to_date_time: DateTime constructor has thrown exception",
			1);
		zend_object_store_ctor_failed(Z_OBJ_P(return_value));
		zval_ptr_dtor(return_value);
		zval_ptr_dtor(&ts_zval);

		RETVAL_FALSE;
		goto error;
	}
	zval_ptr_dtor(&ts_zval);

	/* due to bug #40743, we have to set the time zone again */
	zend_call_method_with_1_params(return_value, NULL, NULL, "settimezone",
			&retval, timezone_zval);
	if (Z_ISUNDEF(retval) || Z_TYPE(retval) == IS_FALSE) {
		intl_errors_set(CALENDAR_ERROR_P(co), U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_to_date_time: call to DateTime::setTimeZone has failed",
			1);
		zval_ptr_dtor(return_value);
		RETVAL_FALSE;
		goto error;
	}

error:
	zval_ptr_dtor(timezone_zval);
	zval_ptr_dtor(&retval);
}

U_CFUNC PHP_FUNCTION(intlcal_get_error_code)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_error_code: bad arguments", 0);
		RETURN_FALSE;
	}

	/* Fetch the object (without resetting its last error code ). */
	co = Z_INTL_CALENDAR_P(object);
	if (co == NULL)
		RETURN_FALSE;

	RETURN_LONG((zend_long)CALENDAR_ERROR_CODE(co));
}

U_CFUNC PHP_FUNCTION(intlcal_get_error_message)
{
	zend_string* message = NULL;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, Calendar_ce_ptr) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_get_error_message: bad arguments", 0 );
		RETURN_FALSE;
	}


	/* Fetch the object (without resetting its last error code ). */
	co = Z_INTL_CALENDAR_P(object);
	if (co == NULL)
		RETURN_FALSE;

	/* Return last error message. */
	message = intl_error_get_message(CALENDAR_ERROR_P(co));
	RETURN_STR(message);
}
