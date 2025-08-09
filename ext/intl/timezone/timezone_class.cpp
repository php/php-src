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

#include "../intl_cppshims.h"
#include <memory>

#include <unicode/timezone.h>
#include <unicode/calendar.h>
#include "../intl_convertcpp.h"
#include "../intl_common.h"

#include "../common/common_date.h"

extern "C" {
#include "../intl_convert.h"
#define USE_TIMEZONE_POINTER 1
#include "timezone_class.h"
#include "timezone_arginfo.h"
#include <zend_exceptions.h>
#include <zend_interfaces.h>
#include <ext/date/php_date.h>
}

using icu::Calendar;

/* {{{ Global variables */
U_CDECL_BEGIN
zend_class_entry *TimeZone_ce_ptr = NULL;
zend_object_handlers TimeZone_handlers;
U_CDECL_END
/* }}} */

/* {{{ timezone_object_construct */
U_CFUNC void timezone_object_construct(const TimeZone *zone, zval *object, int owned)
{
	TimeZone_object	*to;

	object_init_ex(object, TimeZone_ce_ptr);
	TIMEZONE_METHOD_FETCH_OBJECT_NO_CHECK; /* fetch zend object from zval "object" into "to" */
	to->utimezone = zone;
	to->should_delete = owned;
}
/* }}} */

/* {{{ timezone_convert_to_datetimezone
 *	   Convert from TimeZone to DateTimeZone object */
U_CFUNC zval *timezone_convert_to_datetimezone(const TimeZone *timeZone,
											   intl_error *outside_error,
											   zval *ret)
{
	UnicodeString id;

	timeZone->getID(id);
	if (id.isBogus()) {
		intl_errors_set(outside_error, U_ILLEGAL_ARGUMENT_ERROR,
			"could not obtain TimeZone id");
		return nullptr;
	}

	if (id.compare(0, 3, UnicodeString("GMT", sizeof("GMT")-1, US_INV)) == 0) {
		/* The DateTimeZone constructor doesn't support offset time zones,
		* so we must mess with DateTimeZone structure ourselves */
		object_init_ex(ret, php_date_get_timezone_ce());
		php_timezone_obj *tzobj = Z_PHPTIMEZONE_P(ret);

		tzobj->initialized	  = true;
		tzobj->type			  = TIMELIB_ZONETYPE_OFFSET;
		//convert offset from milliseconds to seconds
		tzobj->tzi.utc_offset = timeZone->getRawOffset() / 1000;
	} else {
		zend_string *u8str = intl_charFromString(id, &INTL_ERROR_CODE(*outside_error));
		if (!u8str) {
			intl_errors_set(outside_error, INTL_ERROR_CODE(*outside_error),
				"could not convert id to UTF-8");
			return nullptr;
		}

		zval arg;
		ZVAL_STR(&arg, u8str);
		/* Instantiate the object and call the constructor */
		zend_result status = object_init_with_constructor(ret, php_date_get_timezone_ce(), 1, &arg, nullptr);
		zval_ptr_dtor(&arg);
		if (UNEXPECTED(status == FAILURE)) {
			zend_throw_exception(IntlException_ce_ptr, "DateTimeZone constructor threw exception", 0);
			return nullptr;
		}
	}

	return ret;
}
/* }}} */

static void timezone_throw_exception_with_call_location(const char *msg, const char *add_info)
{
	zend_string *fn = get_active_function_or_method_name();
	zend_throw_error(IntlException_ce_ptr, "%s(): %s%s%s%s",
		ZSTR_VAL(fn), msg,
		add_info ? "\"" : "",
		add_info ? add_info : "",
		add_info ? "\"" : ""
	);
	zend_string_release_ex(fn, false);
}

/* {{{ timezone_process_timezone_argument
 * TimeZone argument processor. outside_error may be nullptr (for static functions/constructors) */
U_CFUNC TimeZone *timezone_process_timezone_argument(
	zend_object *timezone_object, zend_string *timezone_string, intl_error *outside_error)
{
	std::unique_ptr<TimeZone>	timeZone;
	bool free_string = false;

	if (timezone_object == nullptr && timezone_string == nullptr) {
		timelib_tzinfo *tzinfo = get_timezone_info();
		timezone_string = zend_string_init(tzinfo->name, strlen(tzinfo->name), false);
		free_string = true;
	}

	if (timezone_object != nullptr) {
		if (instanceof_function(timezone_object->ce, TimeZone_ce_ptr)) {
			const TimeZone_object *to = php_intl_timezone_fetch_object(timezone_object);

			if (UNEXPECTED(to->utimezone == nullptr)) {
				timezone_throw_exception_with_call_location(
					"passed IntlTimeZone is not properly constructed",nullptr);
				return nullptr;
			}
			timeZone = std::unique_ptr<TimeZone>(to->utimezone->clone());
			if (UNEXPECTED(timeZone == nullptr)) {
				timezone_throw_exception_with_call_location("could not clone TimeZone", nullptr);
				return nullptr;
			}
			// well, this is included by the centralized C intl part so the "smart" part can't go further
			return timeZone.release();
		} else if (instanceof_function(timezone_object->ce, php_date_get_timezone_ce())) {
			php_timezone_obj *tz_obj = php_timezone_obj_from_obj(timezone_object);

			return timezone_convert_datetimezone(tz_obj->type, tz_obj, false, outside_error);
		} else {
			zval tmp;
			zend_result status = timezone_object->handlers->cast_object(timezone_object, &tmp, IS_STRING);
			if (EXPECTED(status == SUCCESS)) {
				timezone_string = Z_STR(tmp);
				free_string = true;
			} else {
				if (!EG(exception)) {
					// TODO Proper type error
					zend_throw_error(nullptr, "Object of class %s could not be converted to string", ZSTR_VAL(timezone_object->ce->name));
				}
				return nullptr;
			}
		}
	}

	ZEND_ASSERT(timezone_string != nullptr);
	UnicodeString id;
	UErrorCode status = U_ZERO_ERROR; /* outside_error may be nullptr */

	if (UNEXPECTED(intl_stringFromChar(id, ZSTR_VAL(timezone_string), ZSTR_LEN(timezone_string), &status) == FAILURE)) {
		timezone_throw_exception_with_call_location("Time zone identifier given is not a valid UTF-8 string", nullptr);
		if (free_string) {
			zend_string_release_ex(timezone_string, false);
		}
		return nullptr;
	}

	timeZone = std::unique_ptr<TimeZone>(TimeZone::createTimeZone(id));
	if (UNEXPECTED(timeZone == nullptr)) {
		timezone_throw_exception_with_call_location("Could not create time zone",nullptr);
		if (free_string) {
			zend_string_release_ex(timezone_string, false);
		}
		return nullptr;
	}
	if (UNEXPECTED(*timeZone == TimeZone::getUnknown())) {
		timezone_throw_exception_with_call_location("No such time zone: ", ZSTR_VAL(timezone_string));
		if (free_string) {
			zend_string_release_ex(timezone_string, false);
		}
		return nullptr;
	}
	if (free_string) {
		zend_string_release_ex(timezone_string, false);
	}
	// well, this is included by the centralized C intl part so the "smart" part can't go further
	return timeZone.release();
}
/* }}} */

/* {{{ clone handler for TimeZone */
static zend_object *TimeZone_clone_obj(zend_object *object)
{
	TimeZone_object *to_orig = php_intl_timezone_fetch_object(object);
	zend_object     *ret_val = TimeZone_ce_ptr->create_object(object->ce);
	TimeZone_object  *to_new = php_intl_timezone_fetch_object(ret_val);

	zend_objects_clone_members(&to_new->zo, &to_orig->zo);

	if (to_orig->utimezone != NULL) {
		TimeZone *newTimeZone = to_orig->utimezone->clone();
		to_new->should_delete = true;
		if (!newTimeZone) {
			zend_throw_error(NULL, "Failed to clone IntlTimeZone");
		} else {
			to_new->utimezone = newTimeZone;
		}
	} else {
		zend_throw_error(NULL, "Cannot clone uninitialized IntlTimeZone");
	}

	return ret_val;
}
/* }}} */

/* {{{ compare_objects handler for TimeZone
 *     Can't be used for >, >=, <, <= comparisons */
static int TimeZone_compare_objects(zval *object1, zval *object2)
{
	TimeZone_object		*to1,
						*to2;

	ZEND_COMPARE_OBJECTS_FALLBACK(object1, object2);

	to1 = Z_INTL_TIMEZONE_P(object1);
	to2 = Z_INTL_TIMEZONE_P(object2);

	if (to1->utimezone == NULL || to2->utimezone == NULL) {
		zend_throw_exception(NULL, "Comparison with at least one unconstructed "
				"IntlTimeZone operand", 0);
		/* intentionally not returning */
	} else {
		if (*to1->utimezone == *to2->utimezone) {
			return 0;
		}
	}

	return ZEND_UNCOMPARABLE;
}
/* }}} */

/* {{{ get_debug_info handler for TimeZone */
static HashTable *TimeZone_get_debug_info(zend_object *object, int *is_temp)
{
	zval			zv;
	TimeZone_object	*to;
	const TimeZone	*tz;
	UnicodeString	ustr;
	zend_string     *u8str;
	HashTable 		*debug_info;
	UErrorCode		uec = U_ZERO_ERROR;

	*is_temp = 1;

	debug_info = zend_new_array(8);

	to = php_intl_timezone_fetch_object(object);
	tz = to->utimezone;

	if (tz == NULL) {
		ZVAL_FALSE(&zv);
		zend_hash_str_update(debug_info, "valid", sizeof("valid") - 1, &zv);
		return debug_info;
	}

	ZVAL_TRUE(&zv);
	zend_hash_str_update(debug_info, "valid", sizeof("valid") - 1, &zv);

	tz->getID(ustr);
	u8str = intl_convert_utf16_to_utf8(
		ustr.getBuffer(), ustr.length(), &uec);
	if (!u8str) {
		return debug_info;
	}
	ZVAL_NEW_STR(&zv, u8str);
	zend_hash_str_update(debug_info, "id", sizeof("id") - 1, &zv);

	int32_t rawOffset, dstOffset;
	UDate now = Calendar::getNow();
	tz->getOffset(now, false, rawOffset, dstOffset, uec);
	if (U_FAILURE(uec)) {
		return debug_info;
	}

	ZVAL_LONG(&zv, (zend_long)rawOffset);
	zend_hash_str_update(debug_info,"rawOffset", sizeof("rawOffset") - 1, &zv);
	ZVAL_LONG(&zv, (zend_long)(rawOffset + dstOffset));
	zend_hash_str_update(debug_info,"currentOffset", sizeof("currentOffset") - 1, &zv);

	return debug_info;
}
/* }}} */

/* {{{ void TimeZone_object_init(TimeZone_object* to)
 * Initialize internals of TImeZone_object not specific to zend standard objects.
 */
static void TimeZone_object_init(TimeZone_object *to)
{
	intl_error_init(TIMEZONE_ERROR_P(to));
	to->utimezone = NULL;
	to->should_delete = 0;
}
/* }}} */

/* {{{ TimeZone_objects_free */
static void TimeZone_objects_free(zend_object *object)
{
	TimeZone_object* to = php_intl_timezone_fetch_object(object);

	if (to->utimezone && to->should_delete) {
		delete to->utimezone;
		to->utimezone = NULL;
	}
	intl_error_reset(TIMEZONE_ERROR_P(to));

	zend_object_std_dtor(&to->zo);
}
/* }}} */

/* {{{ TimeZone_object_create */
static zend_object *TimeZone_object_create(zend_class_entry *ce)
{
	TimeZone_object* intern = (TimeZone_object*)zend_object_alloc(sizeof(TimeZone_object), ce);

	zend_object_std_init(&intern->zo, ce);
    object_properties_init(&intern->zo, ce);
	TimeZone_object_init(intern);

	return &intern->zo;
}
/* }}} */

/* {{{ timezone_register_IntlTimeZone_class
 * Initialize 'IntlTimeZone' class
 */
U_CFUNC void timezone_register_IntlTimeZone_class(void)
{
	/* Create and register 'IntlTimeZone' class. */
	TimeZone_ce_ptr = register_class_IntlTimeZone();
	TimeZone_ce_ptr->create_object = TimeZone_object_create;
	TimeZone_ce_ptr->default_object_handlers = &TimeZone_handlers;

	memcpy(&TimeZone_handlers, &std_object_handlers,
		sizeof TimeZone_handlers);
	TimeZone_handlers.offset = XtOffsetOf(TimeZone_object, zo);
	TimeZone_handlers.clone_obj = TimeZone_clone_obj;
	TimeZone_handlers.compare = TimeZone_compare_objects;
	TimeZone_handlers.get_debug_info = TimeZone_get_debug_info;
	TimeZone_handlers.free_obj = TimeZone_objects_free;
}
/* }}} */
