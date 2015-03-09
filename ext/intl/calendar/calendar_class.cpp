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

#include "../intl_cppshims.h"

#include <unicode/calendar.h>
#include <unicode/gregocal.h>

extern "C" {
#define USE_TIMEZONE_POINTER 1
#include "../timezone/timezone_class.h"
#define USE_CALENDAR_POINTER 1
#include "calendar_class.h"
#include "calendar_methods.h"
#include "gregoriancalendar_methods.h"
#include <zend_exceptions.h>
#include <assert.h>
}

/* {{{ Global variables */
zend_class_entry *Calendar_ce_ptr;
zend_class_entry *GregorianCalendar_ce_ptr;
zend_object_handlers Calendar_handlers;
/* }}} */

U_CFUNC	void calendar_object_create(zval *object,
									Calendar *calendar)
{
	UClassID classId = calendar->getDynamicClassID();
	zend_class_entry *ce;

	//if (dynamic_cast<GregorianCalendar*>(calendar) != NULL) {
	if (classId == GregorianCalendar::getStaticClassID()) {
		ce = GregorianCalendar_ce_ptr;
	} else {
		ce = Calendar_ce_ptr;
	}

	object_init_ex(object, ce);
	calendar_object_construct(object, calendar);
}

U_CFUNC Calendar *calendar_fetch_native_calendar(zval *object)
{
	Calendar_object *co = Z_INTL_CALENDAR_P(object);

	return co->ucal;
}

U_CFUNC void calendar_object_construct(zval *object,
									   Calendar *calendar)
{
	Calendar_object *co;

	CALENDAR_METHOD_FETCH_OBJECT_NO_CHECK; //populate to from object
	assert(co->ucal == NULL);
	co->ucal = (Calendar*)calendar;
}

/* {{{ clone handler for Calendar */
static zend_object *Calendar_clone_obj(zval *object)
{
	Calendar_object		*co_orig,
						*co_new;
	zend_object 	    *ret_val;
	intl_error_reset(NULL);

	co_orig = Z_INTL_CALENDAR_P(object);
	intl_error_reset(INTL_DATA_ERROR_P(co_orig));

	ret_val = Calendar_ce_ptr->create_object(Z_OBJCE_P(object));
	co_new  = php_intl_calendar_fetch_object(ret_val);

	zend_objects_clone_members(&co_new->zo, &co_orig->zo);

	if (co_orig->ucal != NULL) {
		Calendar	*newCalendar;

		newCalendar = co_orig->ucal->clone();
		if (!newCalendar) {
			zend_string *err_msg;
			intl_errors_set_code(CALENDAR_ERROR_P(co_orig),
				U_MEMORY_ALLOCATION_ERROR);
			intl_errors_set_custom_msg(CALENDAR_ERROR_P(co_orig),
				"Could not clone IntlCalendar", 0);
			err_msg = intl_error_get_message(CALENDAR_ERROR_P(co_orig));
			zend_throw_exception(NULL, err_msg->val, 0);
			zend_string_free(err_msg);
		} else {
			co_new->ucal = newCalendar;
		}
	} else {
		zend_throw_exception(NULL, "Cannot clone unconstructed IntlCalendar", 0);
	}

	return ret_val;
}
/* }}} */

static const struct {
	UCalendarDateFields field;
	const char			*name;
} debug_info_fields[] = {
	{UCAL_ERA,					"era"},
	{UCAL_YEAR,					"year"},
	{UCAL_MONTH,				"month"},
	{UCAL_WEEK_OF_YEAR,			"week of year"},
	{UCAL_WEEK_OF_MONTH,		"week of month"},
	{UCAL_DAY_OF_YEAR,			"day of year"},
	{UCAL_DAY_OF_MONTH,			"day of month"},
	{UCAL_DAY_OF_WEEK,			"day of week"},
	{UCAL_DAY_OF_WEEK_IN_MONTH,	"day of week in month"},
	{UCAL_AM_PM,				"AM/PM"},
	{UCAL_HOUR,					"hour"},
	{UCAL_HOUR_OF_DAY,			"hour of day"},
	{UCAL_MINUTE,				"minute"},
	{UCAL_SECOND,				"second"},
	{UCAL_MILLISECOND,			"millisecond"},
	{UCAL_ZONE_OFFSET,			"zone offset"},
	{UCAL_DST_OFFSET,			"DST offset"},
	{UCAL_YEAR_WOY,				"year for week of year"},
	{UCAL_DOW_LOCAL,			"localized day of week"},
	{UCAL_EXTENDED_YEAR,		"extended year"},
	{UCAL_JULIAN_DAY,			"julian day"},
	{UCAL_MILLISECONDS_IN_DAY,	"milliseconds in day"},
	{UCAL_IS_LEAP_MONTH,		"is leap month"},
};

/* {{{ get_debug_info handler for Calendar */
static HashTable *Calendar_get_debug_info(zval *object, int *is_temp)
{
	zval			zv,
					zfields;
	Calendar_object	*co;
	const Calendar	*cal;
	HashTable		*debug_info;

	*is_temp = 1;

	ALLOC_HASHTABLE(debug_info);
	zend_hash_init(debug_info, 8, NULL, ZVAL_PTR_DTOR, 0);

	co  = Z_INTL_CALENDAR_P(object);
	cal = co->ucal;

	if (cal == NULL) {
		ZVAL_FALSE(&zv);
		zend_hash_str_update(debug_info, "valid", sizeof("valid") - 1, &zv);
		return debug_info;
	}
	ZVAL_TRUE(&zv);
	zend_hash_str_update(debug_info, "valid", sizeof("valid") - 1, &zv);

	ZVAL_STRING(&zv, const_cast<char*>(cal->getType()));
	zend_hash_str_update(debug_info, "type", sizeof("type") - 1, &zv);
	{
		zval		   ztz,
					   ztz_debug;
		int			   is_tmp;
		HashTable	   *debug_info_tz;

		timezone_object_construct(&cal->getTimeZone(), &ztz , 0);
		debug_info = Z_OBJ_HANDLER(ztz, get_debug_info)(&ztz, &is_tmp);
		assert(is_tmp == 1);

		array_init(&ztz_debug);
		zend_hash_copy(Z_ARRVAL(ztz_debug), debug_info_tz, zval_add_ref);
		zend_hash_destroy(debug_info_tz);
		FREE_HASHTABLE(debug_info_tz);

		zend_hash_str_update(debug_info, "timeZone", sizeof("timeZone") - 1, &ztz_debug);
	}

	{
		UErrorCode	uec		= U_ZERO_ERROR;
		Locale		locale	= cal->getLocale(ULOC_VALID_LOCALE, uec);
		if (U_SUCCESS(uec)) {
			ZVAL_STRING(&zv, const_cast<char*>(locale.getName()));
			zend_hash_str_update(debug_info, "locale", sizeof("locale") - 1, &zv);
		} else {
			ZVAL_STRING(&zv, const_cast<char*>(u_errorName(uec)));
			zend_hash_str_update(debug_info, "locale", sizeof("locale") - 1, &zv);
		}
	}

	array_init_size(&zfields, UCAL_FIELD_COUNT);

	for (int i = 0;
			 i < sizeof(debug_info_fields) / sizeof(*debug_info_fields);
			 i++) {
		UErrorCode	uec		= U_ZERO_ERROR;
		const char	*name	= debug_info_fields[i].name;
		int32_t		res		= cal->get(debug_info_fields[i].field, uec);
		if (U_SUCCESS(uec)) {
			add_assoc_long(&zfields, name, (zend_long)res);
		} else {
			add_assoc_string(&zfields, name, const_cast<char*>(u_errorName(uec)));
		}
	}

	zend_hash_str_update(debug_info, "fields", sizeof("fields") - 1, &zfields);

	return debug_info;
}
/* }}} */

/* {{{ void calendar_object_init(Calendar_object* to)
 * Initialize internals of Calendar_object not specific to zend standard objects.
 */
static void calendar_object_init(Calendar_object *co)
{
	intl_error_init(CALENDAR_ERROR_P(co));
	co->ucal = NULL;
}
/* }}} */

/* {{{ Calendar_objects_dtor */
static void Calendar_objects_dtor(zend_object *object)
{
	zend_objects_destroy_object(object);
}
/* }}} */

/* {{{ Calendar_objects_free */
static void Calendar_objects_free(zend_object *object)
{
	Calendar_object* co = php_intl_calendar_fetch_object(object);

	if (co->ucal) {
		delete co->ucal;
		co->ucal = NULL;
	}
	intl_error_reset(CALENDAR_ERROR_P(co));

	zend_object_std_dtor(&co->zo);
}
/* }}} */

/* {{{ Calendar_object_create */
static zend_object *Calendar_object_create(zend_class_entry *ce)
{
	Calendar_object*	intern;

	intern = (Calendar_object*)ecalloc(1, sizeof(Calendar_object) + sizeof(zval) * (ce->default_properties_count - 1));

	zend_object_std_init(&intern->zo, ce);
    object_properties_init((zend_object*) intern, ce);
	calendar_object_init(intern);


	intern->zo.handlers = &Calendar_handlers;

	return &intern->zo;
}
/* }}} */

/* {{{ Calendar methods arguments info */

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_field, 0, 0, 1)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_dow, 0, 0, 1)
	ZEND_ARG_INFO(0, dayOfWeek)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_other_cal, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, calendar, IntlCalendar, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_date, 0, 0, 1)
	ZEND_ARG_INFO(0, date)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_date_optional, 0, 0, 0)
	ZEND_ARG_INFO(0, date)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_createInstance, 0, 0, 0)
	ZEND_ARG_INFO(0, timeZone)
	ZEND_ARG_INFO(0, locale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_get_keyword_values_for_locale, 0, 0, 3)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, commonlyUsed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_add, 0, 0, 2)
	ZEND_ARG_INFO(0, field)
	ZEND_ARG_INFO(0, amount)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_setTimeZone, 0, 0, 1)
	ZEND_ARG_INFO(0, timeZone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_set, 0, 0, 2)
	ZEND_ARG_INFO(0, fieldOrYear)
	ZEND_ARG_INFO(0, valueOrMonth)
	ZEND_ARG_INFO(0, dayOfMonth)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_roll, 0, 0, 2)
	ZEND_ARG_INFO(0, field)
	ZEND_ARG_INFO(0, amountOrUpOrDown)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_clear, 0, 0, 0)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_field_difference, 0, 0, 2)
	ZEND_ARG_INFO(0, when)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_get_locale, 0, 0, 1)
	ZEND_ARG_INFO(0, localeType)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_setLenient, 0, 0, 1)
	ZEND_ARG_INFO(0, isLenient)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_set_minimal_days_in_first_week, 0, 0, 1)
	ZEND_ARG_INFO(0, numberOfDays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_from_date_time, 0, 0, 1)
	ZEND_ARG_INFO(0, dateTime)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_wall_time_option, 0, 0, 1)
	ZEND_ARG_INFO(0, wallTimeOption)
ZEND_END_ARG_INFO()

/* Gregorian Calendar */
ZEND_BEGIN_ARG_INFO_EX(ainfo_gregcal___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, timeZoneOrYear)
	ZEND_ARG_INFO(0, localeOrMonth)
	ZEND_ARG_INFO(0, dayOfMonth)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_gregcal_isLeapYear, 0, 0, 1)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ Calendar_class_functions
 * Every 'IntlCalendar' class method has an entry in this table
 */
static const zend_function_entry Calendar_class_functions[] = {
	PHP_ME(IntlCalendar,				__construct,				ainfo_cal_void,						ZEND_ACC_PRIVATE)
	PHP_ME_MAPPING(createInstance,		intlcal_create_instance,	ainfo_cal_createInstance,			ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 42
	PHP_ME_MAPPING(getKeywordValuesForLocale, intlcal_get_keyword_values_for_locale, ainfo_cal_get_keyword_values_for_locale, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(getNow,				intlcal_get_now,			ainfo_cal_void,						ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getAvailableLocales,	intlcal_get_available_locales, ainfo_cal_void,					ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(get,					intlcal_get,				ainfo_cal_field,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getTime,				intlcal_get_time,			ainfo_cal_void,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setTime,				intlcal_set_time,			ainfo_cal_date,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(add,					intlcal_add,				ainfo_cal_add,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setTimeZone,			intlcal_set_time_zone,		ainfo_cal_setTimeZone,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(after,				intlcal_after,				ainfo_cal_other_cal,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(before,				intlcal_before,				ainfo_cal_other_cal,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(set,					intlcal_set,				ainfo_cal_set,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(roll,				intlcal_roll,				ainfo_cal_roll,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(clear,				intlcal_clear,				ainfo_cal_clear,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(fieldDifference,		intlcal_field_difference,	ainfo_cal_field_difference,			ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getActualMaximum,	intlcal_get_actual_maximum,	ainfo_cal_field,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getActualMinimum,	intlcal_get_actual_minimum,	ainfo_cal_field,					ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	PHP_ME_MAPPING(getDayOfWeekType,	intlcal_get_day_of_week_type, ainfo_cal_dow,					ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(getFirstDayOfWeek,	intlcal_get_first_day_of_week, ainfo_cal_void,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getGreatestMinimum,	intlcal_get_greatest_minimum, ainfo_cal_field,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getLeastMaximum,		intlcal_get_least_maximum,	ainfo_cal_field,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getLocale,			intlcal_get_locale,			ainfo_cal_get_locale,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getMaximum,			intlcal_get_maximum,		ainfo_cal_field,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getMinimalDaysInFirstWeek, intlcal_get_minimal_days_in_first_week, ainfo_cal_void,	ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getMinimum,			intlcal_get_minimum,		ainfo_cal_field,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getTimeZone,			intlcal_get_time_zone,		ainfo_cal_void,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getType,				intlcal_get_type,			ainfo_cal_void,						ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	PHP_ME_MAPPING(getWeekendTransition,intlcal_get_weekend_transition, ainfo_cal_dow,					ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(inDaylightTime,		intlcal_in_daylight_time,	ainfo_cal_void,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(isEquivalentTo,		intlcal_is_equivalent_to,	ainfo_cal_other_cal,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(isLenient,			intlcal_is_lenient,			ainfo_cal_void,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(isSet,				intlcal_is_set,				ainfo_cal_field,					ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	PHP_ME_MAPPING(isWeekend,			intlcal_is_weekend,			ainfo_cal_date_optional,			ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(setFirstDayOfWeek,	intlcal_set_first_day_of_week, ainfo_cal_dow,					ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setLenient,			intlcal_set_lenient,		ainfo_cal_setLenient,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setMinimalDaysInFirstWeek,intlcal_set_minimal_days_in_first_week,ainfo_cal_set_minimal_days_in_first_week,ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(equals,				intlcal_equals,				ainfo_cal_other_cal,				ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM >= 49
	PHP_ME_MAPPING(getRepeatedWallTimeOption,intlcal_get_repeated_wall_time_option,ainfo_cal_void,		ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getSkippedWallTimeOption,intlcal_get_skipped_wall_time_option,ainfo_cal_void,		ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setRepeatedWallTimeOption,intlcal_set_repeated_wall_time_option,ainfo_cal_wall_time_option,ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setSkippedWallTimeOption,intlcal_set_skipped_wall_time_option,ainfo_cal_wall_time_option,ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(fromDateTime,		intlcal_from_date_time,		ainfo_cal_from_date_time,			ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(toDateTime,			intlcal_to_date_time,		ainfo_cal_void,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getErrorCode,		intlcal_get_error_code,		ainfo_cal_void,						ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getErrorMessage,		intlcal_get_error_message,	ainfo_cal_void,						ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ GregorianCalendar_class_functions
 */
static const zend_function_entry GregorianCalendar_class_functions[] = {
	PHP_ME(IntlGregorianCalendar,		__construct,				ainfo_gregcal___construct,			ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setGregorianChange,	intlgregcal_set_gregorian_change, ainfo_cal_date,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getGregorianChange,	intlgregcal_get_gregorian_change, ainfo_cal_void,				ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(isLeapYear,			intlgregcal_is_leap_year,	ainfo_gregcal_isLeapYear,			ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */


/* {{{ calendar_register_IntlCalendar_class
 * Initialize 'IntlCalendar' class
 */
void calendar_register_IntlCalendar_class(void)
{
	zend_class_entry ce;

	/* Create and register 'IntlCalendar' class. */
	INIT_CLASS_ENTRY(ce, "IntlCalendar", Calendar_class_functions);
	ce.create_object = Calendar_object_create;
	Calendar_ce_ptr = zend_register_internal_class(&ce);
	if (!Calendar_ce_ptr) {
		//can't happen now without bigger problems before
		php_error_docref0(NULL, E_ERROR,
			"IntlCalendar: class registration has failed.");
		return;
	}
	memcpy( &Calendar_handlers, zend_get_std_object_handlers(),
		sizeof Calendar_handlers);
	Calendar_handlers.offset = XtOffsetOf(Calendar_object, zo);
	Calendar_handlers.clone_obj = Calendar_clone_obj;
	Calendar_handlers.get_debug_info = Calendar_get_debug_info;
	Calendar_handlers.dtor_obj = Calendar_objects_dtor;
	Calendar_handlers.free_obj = Calendar_objects_free;

	/* Create and register 'IntlGregorianCalendar' class. */
	INIT_CLASS_ENTRY(ce, "IntlGregorianCalendar", GregorianCalendar_class_functions);
	GregorianCalendar_ce_ptr = zend_register_internal_class_ex(&ce,
		Calendar_ce_ptr);
	if (!GregorianCalendar_ce_ptr) {
		//can't happen know without bigger problems before
		php_error_docref0(NULL, E_ERROR,
			"IntlGregorianCalendar: class registration has failed.");
		return;
	}

	/* Declare 'IntlCalendar' class constants */
#define CALENDAR_DECL_LONG_CONST(name, val) \
	zend_declare_class_constant_long(Calendar_ce_ptr, name, sizeof(name) - 1, \
		val)

	CALENDAR_DECL_LONG_CONST("FIELD_ERA",					UCAL_ERA);
	CALENDAR_DECL_LONG_CONST("FIELD_YEAR",					UCAL_YEAR);
	CALENDAR_DECL_LONG_CONST("FIELD_MONTH",					UCAL_MONTH);
	CALENDAR_DECL_LONG_CONST("FIELD_WEEK_OF_YEAR",			UCAL_WEEK_OF_YEAR);
	CALENDAR_DECL_LONG_CONST("FIELD_WEEK_OF_MONTH",			UCAL_WEEK_OF_MONTH);
	CALENDAR_DECL_LONG_CONST("FIELD_DATE",					UCAL_DATE);
	CALENDAR_DECL_LONG_CONST("FIELD_DAY_OF_YEAR",			UCAL_DAY_OF_YEAR);
	CALENDAR_DECL_LONG_CONST("FIELD_DAY_OF_WEEK",			UCAL_DAY_OF_WEEK);
	CALENDAR_DECL_LONG_CONST("FIELD_DAY_OF_WEEK_IN_MONTH",	UCAL_DAY_OF_WEEK_IN_MONTH);
	CALENDAR_DECL_LONG_CONST("FIELD_AM_PM",					UCAL_AM_PM);
	CALENDAR_DECL_LONG_CONST("FIELD_HOUR",					UCAL_HOUR);
	CALENDAR_DECL_LONG_CONST("FIELD_HOUR_OF_DAY",			UCAL_HOUR_OF_DAY);
	CALENDAR_DECL_LONG_CONST("FIELD_HOUR",					UCAL_HOUR);
	CALENDAR_DECL_LONG_CONST("FIELD_HOUR_OF_DAY",			UCAL_HOUR_OF_DAY);
	CALENDAR_DECL_LONG_CONST("FIELD_MINUTE",				UCAL_MINUTE);
	CALENDAR_DECL_LONG_CONST("FIELD_SECOND",				UCAL_SECOND);
	CALENDAR_DECL_LONG_CONST("FIELD_MILLISECOND",			UCAL_MILLISECOND);
	CALENDAR_DECL_LONG_CONST("FIELD_ZONE_OFFSET",			UCAL_ZONE_OFFSET);
	CALENDAR_DECL_LONG_CONST("FIELD_DST_OFFSET",			UCAL_DST_OFFSET);
	CALENDAR_DECL_LONG_CONST("FIELD_YEAR_WOY",				UCAL_YEAR_WOY);
	CALENDAR_DECL_LONG_CONST("FIELD_DOW_LOCAL",				UCAL_DOW_LOCAL);
	CALENDAR_DECL_LONG_CONST("FIELD_EXTENDED_YEAR",			UCAL_EXTENDED_YEAR);
	CALENDAR_DECL_LONG_CONST("FIELD_JULIAN_DAY",			UCAL_JULIAN_DAY);
	CALENDAR_DECL_LONG_CONST("FIELD_MILLISECONDS_IN_DAY",	UCAL_MILLISECONDS_IN_DAY);
	CALENDAR_DECL_LONG_CONST("FIELD_IS_LEAP_MONTH",			UCAL_IS_LEAP_MONTH);
	CALENDAR_DECL_LONG_CONST("FIELD_FIELD_COUNT",			UCAL_FIELD_COUNT);
	CALENDAR_DECL_LONG_CONST("FIELD_DAY_OF_MONTH",			UCAL_DAY_OF_MONTH);

	CALENDAR_DECL_LONG_CONST("DOW_SUNDAY",					UCAL_SUNDAY);
	CALENDAR_DECL_LONG_CONST("DOW_MONDAY",					UCAL_MONDAY);
	CALENDAR_DECL_LONG_CONST("DOW_TUESDAY",					UCAL_TUESDAY);
	CALENDAR_DECL_LONG_CONST("DOW_WEDNESDAY",				UCAL_WEDNESDAY);
	CALENDAR_DECL_LONG_CONST("DOW_THURSDAY",				UCAL_THURSDAY);
	CALENDAR_DECL_LONG_CONST("DOW_FRIDAY",					UCAL_FRIDAY);
	CALENDAR_DECL_LONG_CONST("DOW_SATURDAY",				UCAL_SATURDAY);

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	CALENDAR_DECL_LONG_CONST("DOW_TYPE_WEEKDAY",			UCAL_WEEKDAY);
	CALENDAR_DECL_LONG_CONST("DOW_TYPE_WEEKEND",			UCAL_WEEKEND);
	CALENDAR_DECL_LONG_CONST("DOW_TYPE_WEEKEND_OFFSET",		UCAL_WEEKEND_ONSET);
	CALENDAR_DECL_LONG_CONST("DOW_TYPE_WEEKEND_CEASE",		UCAL_WEEKEND_CEASE);
#endif

#if U_ICU_VERSION_MAJOR_NUM >= 49
	CALENDAR_DECL_LONG_CONST("WALLTIME_FIRST",				UCAL_WALLTIME_FIRST);
	CALENDAR_DECL_LONG_CONST("WALLTIME_LAST",				UCAL_WALLTIME_LAST);
	CALENDAR_DECL_LONG_CONST("WALLTIME_NEXT_VALID",			UCAL_WALLTIME_NEXT_VALID);
#endif
}
/* }}} */
