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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

#define ZEND_VALUE_ERROR_INVALID_FIELD(argument, zpp_arg_position) \
	if (argument < 0 || argument >= UCAL_FIELD_COUNT) { \
		zend_argument_value_error(getThis() ? ((zpp_arg_position)-1) : zpp_arg_position, \
			"must be a valid field"); \
		RETURN_THROWS(); \
	}

#define ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(argument, zpp_arg_position) \
	if (argument < INT32_MIN || argument > INT32_MAX) { \
		zend_argument_value_error(getThis() ? ((zpp_arg_position)-1) : zpp_arg_position, \
			"must be between %d and %d", INT32_MIN, INT32_MAX); \
		RETURN_THROWS(); \
	}

#define ZEND_VALUE_ERROR_INVALID_DAY_OF_WEEK(argument, zpp_arg_position) \
	if (argument < UCAL_SUNDAY || argument > UCAL_SATURDAY) { \
		zend_argument_value_error(getThis() ? ((zpp_arg_position)-1) : zpp_arg_position, \
			"must be a valid day of the week"); \
		RETURN_THROWS(); \
	}

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
		RETURN_THROWS();
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

	int32_t count(UErrorCode& status) const override {
		return uenum_count(uenum, &status);
	}

	const UnicodeString* snext(UErrorCode& status) override
	{
		int32_t length;
		const UChar* str = uenum_unext(uenum, &length, &status);
		if (str == 0 || U_FAILURE(status)) {
			return 0;
		}
		return &unistr.setTo(str, length);
	}

	const char* next(int32_t *resultLength, UErrorCode &status) override
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

	void reset(UErrorCode& status) override
	{
		uenum_reset(uenum, &status);
	}

	UClassID getDynamicClassID() const override;

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
	bool	commonly_used;
	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssb",
			&key, &key_len, &locale, &locale_len, &commonly_used) == FAILURE) {
		RETURN_THROWS();
	}

	StringEnumeration *se = Calendar::getKeywordValuesForLocale(key,
		Locale::createFromName(locale), (UBool)commonly_used,
		status);
	if (se == NULL) {
		intl_error_set(NULL, status, "intlcal_get_keyword_values_for_locale: "
			"error calling underlying method", 0);
		RETURN_FALSE;
	}

	IntlIterator_from_StringEnumeration(se, return_value);
}

U_CFUNC PHP_FUNCTION(intlcal_get_now)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_DOUBLE((double)Calendar::getNow());
}

U_CFUNC PHP_FUNCTION(intlcal_get_available_locales)
{
	intl_error_reset(NULL);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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
		INTERNAL_FUNCTION_PARAMETERS)
{
	zend_long	field;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &field) == FAILURE) {
		RETURN_THROWS();
	}

	ZEND_VALUE_ERROR_INVALID_FIELD(field, 2);

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t result = (co->ucal->*func)(
		(UCalendarDateFields)field, CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co, "Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get)
{
	_php_intlcal_field_uec_ret_in32t_method(&Calendar::get,
		INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_time)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, Calendar_ce_ptr) == FAILURE) {
		RETURN_THROWS();
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
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	ZEND_VALUE_ERROR_INVALID_FIELD(field, 2);
	ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(amount, 3);

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
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	when_co = Z_INTL_CALENDAR_P(when_object);
	if (when_co->ucal == NULL) {
		zend_argument_error(NULL, 2, "is uninitialized");
		RETURN_THROWS();
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
	zend_long args[6];

	CALENDAR_METHOD_INIT_VARS;

	object = getThis();

	int arg_num = ZEND_NUM_ARGS() - (object ? 0 : 1);

	if (zend_parse_method_parameters(
		ZEND_NUM_ARGS(), object, "Oll|llll",
		&object, Calendar_ce_ptr, &args[0], &args[1], &args[2], &args[3], &args[4], &args[5]
	) == FAILURE) {
		RETURN_THROWS();
	}

	for (int i = 0; i < arg_num; i++) {
		/* Arguments start at 1 */
		ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(args[i], i + 1);
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	if (arg_num == 2) {
		ZEND_VALUE_ERROR_INVALID_FIELD(args[0], 2);
		co->ucal->set((UCalendarDateFields)args[0], (int32_t)args[1]);
	} else if (arg_num == 3) {
		co->ucal->set((int32_t)args[0], (int32_t)args[1], (int32_t)args[2]);
	} else if (arg_num == 4) {
		zend_argument_count_error("IntlCalendar::set() has no variant with exactly 4 parameters");
		RETURN_THROWS();
	} else if (arg_num == 5) {
		co->ucal->set((int32_t)args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3], (int32_t)args[4]);
	} else {
		co->ucal->set((int32_t)args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3], (int32_t)args[4], (int32_t)args[5]);
	}

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_roll)
{
	zval *zvalue;
	zend_long field, value;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Olz", &object, Calendar_ce_ptr, &field, &zvalue) == FAILURE) {
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	ZEND_VALUE_ERROR_INVALID_FIELD(field, 2);

	if (Z_TYPE_P(zvalue) == IS_FALSE || Z_TYPE_P(zvalue) == IS_TRUE) {
		value = Z_TYPE_P(zvalue) == IS_TRUE ? 1 : -1;
		php_error_docref(NULL, E_DEPRECATED, "Passing bool is deprecated, use 1 or -1 instead");
	} else {
		value = zval_get_long(zvalue);
		ZEND_VALUE_ERROR_OUT_OF_BOUND_VALUE(value, 3);
	}

	co->ucal->roll((UCalendarDateFields)field, (int32_t)value, CALENDAR_ERROR_CODE(co));

	INTL_METHOD_CHECK_STATUS(co, "intlcal_roll: Error calling ICU Calendar::roll");

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_clear)
{
	zend_long field;
	bool field_is_null = 1;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(),
			getThis(), "O|l!", &object, Calendar_ce_ptr, &field, &field_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	if (field_is_null) {
		co->ucal->clear();
	} else {
		ZEND_VALUE_ERROR_INVALID_FIELD(field, 2);

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
		RETURN_THROWS();
	}

	ZEND_VALUE_ERROR_INVALID_FIELD(field, 3);

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
		INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_actual_minimum)
{
	_php_intlcal_field_uec_ret_in32t_method(&Calendar::getActualMinimum,
		INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_day_of_week_type)
{
	zend_long	dow;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &dow) == FAILURE) {
		RETURN_THROWS();
	}

	ZEND_VALUE_ERROR_INVALID_DAY_OF_WEEK(dow, 2);

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
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t result = co->ucal->getFirstDayOfWeek(CALENDAR_ERROR_CODE(co));
	INTL_METHOD_CHECK_STATUS(co,
		"intlcal_get_first_day_of_week: Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

static void _php_intlcal_field_ret_in32t_method(
		int32_t (Calendar::*func)(UCalendarDateFields) const,
		INTERNAL_FUNCTION_PARAMETERS)
{
	zend_long	field;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &field) == FAILURE) {
		RETURN_THROWS();
	}

	ZEND_VALUE_ERROR_INVALID_FIELD(field, 2);

	CALENDAR_METHOD_FETCH_OBJECT;

	int32_t result = (co->ucal->*func)((UCalendarDateFields)field);
	INTL_METHOD_CHECK_STATUS(co, "Call to ICU method has failed");

	RETURN_LONG((zend_long)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get_greatest_minimum)
{
	_php_intlcal_field_ret_in32t_method(&Calendar::getGreatestMinimum,
		INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_least_maximum)
{
	_php_intlcal_field_ret_in32t_method(&Calendar::getLeastMaximum,
		INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_locale)
{
	zend_long	locale_type;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ol", &object, Calendar_ce_ptr, &locale_type) == FAILURE) {
		RETURN_THROWS();
	}

	if (locale_type != ULOC_ACTUAL_LOCALE && locale_type != ULOC_VALID_LOCALE) {
		zend_argument_value_error(getThis() ? 1 : 2, "must be either Locale::ACTUAL_LOCALE or Locale::VALID_LOCALE");
		RETURN_THROWS();
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
		INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_minimal_days_in_first_week)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	uint8_t result = co->ucal->getMinimalDaysInFirstWeek();
	INTL_METHOD_CHECK_STATUS(co,
		"intlcal_get_first_day_of_week: Call to ICU method has failed"); /* TODO Is it really a failure? */

	RETURN_LONG((zend_long)result);
}

U_CFUNC PHP_FUNCTION(intlcal_get_minimum)
{
	_php_intlcal_field_ret_in32t_method(&Calendar::getMinimum,
		INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

U_CFUNC PHP_FUNCTION(intlcal_get_time_zone)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		RETURN_THROWS();
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
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	ZEND_VALUE_ERROR_INVALID_DAY_OF_WEEK(dow, 2);

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
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	other_co = Z_INTL_CALENDAR_P(other_object);
	if (other_co->ucal == NULL) {
		zend_argument_error(NULL, 2, "is uninitialized");
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_BOOL((int)co->ucal->isEquivalentTo(*other_co->ucal));
}

U_CFUNC PHP_FUNCTION(intlcal_is_lenient)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	ZEND_VALUE_ERROR_INVALID_FIELD(field, 2);

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_BOOL((int)co->ucal->isSet((UCalendarDateFields)field));
}

U_CFUNC PHP_FUNCTION(intlcal_is_weekend)
{
	double date;
	bool date_is_null = 1;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
				"O|d!", &object, Calendar_ce_ptr, &date, &date_is_null) == FAILURE) {
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	ZEND_VALUE_ERROR_INVALID_DAY_OF_WEEK(dow, 2);

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->setFirstDayOfWeek((UCalendarDaysOfWeek)dow);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_set_lenient)
{
	bool is_lenient;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"Ob", &object, Calendar_ce_ptr, &is_lenient) == FAILURE) {
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	// Use ZEND_VALUE_ERROR_INVALID_DAY_OF_WEEK ?
	if (num_days < 1 || num_days > 7) {
		zend_argument_value_error(getThis() ? 1 : 2, "must be between 1 and 7");
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;
	other_co = Z_INTL_CALENDAR_P(other_object);
	if (other_co->ucal == NULL) {
		zend_argument_error(NULL, 2, "is uninitialized");
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	RETURN_LONG(co->ucal->getRepeatedWallTimeOption());
}

U_CFUNC PHP_FUNCTION(intlcal_get_skipped_wall_time_option)
{
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(),
			"O", &object, Calendar_ce_ptr) == FAILURE) {
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	if (option != UCAL_WALLTIME_FIRST && option != UCAL_WALLTIME_LAST) {
		zend_argument_value_error(getThis() ? 1 : 2, "must be either IntlCalendar::WALLTIME_FIRST or "
			"IntlCalendar::WALLTIME_LAST");
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	if (option != UCAL_WALLTIME_FIRST && option != UCAL_WALLTIME_LAST
			&& option != UCAL_WALLTIME_NEXT_VALID) {
		zend_argument_value_error(getThis() ? 1 : 2, "must be one of IntlCalendar::WALLTIME_FIRST, "
			"IntlCalendar::WALLTIME_LAST, or IntlCalendar::WALLTIME_NEXT_VALID");
		RETURN_THROWS();
	}

	CALENDAR_METHOD_FETCH_OBJECT;

	co->ucal->setSkippedWallTimeOption((UCalendarWallTimeOption)option);

	RETURN_TRUE;
}

U_CFUNC PHP_FUNCTION(intlcal_from_date_time)
{
	zend_object     *date_obj;
	zend_string     *date_str;
	zval			zv_tmp, zv_arg, zv_timestamp;
	php_date_obj	*datetime;
	char			*locale_str = NULL;
	size_t				locale_str_len;
	TimeZone		*timeZone;
	UErrorCode		status = U_ZERO_ERROR;
	Calendar        *cal;
	intl_error_reset(NULL);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(date_obj, php_date_get_date_ce(), date_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(locale_str, locale_str_len)
	ZEND_PARSE_PARAMETERS_END();

	if (date_str) {
		object_init_ex(&zv_tmp, php_date_get_date_ce());
		ZVAL_STR(&zv_arg, date_str);
		zend_call_known_instance_method_with_1_params(Z_OBJCE(zv_tmp)->constructor, Z_OBJ(zv_tmp), NULL, &zv_arg);
		date_obj = Z_OBJ(zv_tmp);
		if (EG(exception)) {
			zend_object_store_ctor_failed(Z_OBJ(zv_tmp));
			goto error;
		}
	}

	datetime = php_date_obj_from_obj(date_obj);
	if (!datetime->time) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"intlcal_from_date_time: DateTime object is unconstructed",
			0);
		goto error;
	}

	zend_call_method_with_0_params(date_obj, php_date_get_date_ce(), NULL, "gettimestamp", &zv_timestamp);
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
	if (date_str) {
		OBJ_RELEASE(date_obj);
	}
}

U_CFUNC PHP_FUNCTION(intlcal_to_date_time)
{
	zval retval;
	CALENDAR_METHOD_INIT_VARS;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, Calendar_ce_ptr) == FAILURE) {
		RETURN_THROWS();
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
	zend_call_known_instance_method_with_2_params(
		Z_OBJCE_P(return_value)->constructor, Z_OBJ_P(return_value), NULL, &ts_zval, timezone_zval);
	if (EG(exception)) {
		zend_object_store_ctor_failed(Z_OBJ_P(return_value));
		zval_ptr_dtor(return_value);
		zval_ptr_dtor(&ts_zval);

		RETVAL_FALSE;
		goto error;
	}
	zval_ptr_dtor(&ts_zval);

	/* due to bug #40743, we have to set the time zone again */
	zend_call_method_with_1_params(Z_OBJ_P(return_value), NULL, NULL, "settimezone",
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
		RETURN_THROWS();
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
		RETURN_THROWS();
	}


	/* Fetch the object (without resetting its last error code ). */
	co = Z_INTL_CALENDAR_P(object);
	if (co == NULL)
		RETURN_FALSE;

	/* Return last error message. */
	message = intl_error_get_message(CALENDAR_ERROR_P(co));
	RETURN_STR(message);
}
