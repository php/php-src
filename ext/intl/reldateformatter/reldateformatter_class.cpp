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
   | Authors: Weilin Du <weilindu@php.net>                                |
   +----------------------------------------------------------------------+
 */

extern "C" {
#include "php.h"
#include "zend_exceptions.h"
}

#include "../formatter/formatter_class.h"

#include <unicode/locid.h>
#include <unicode/udisplaycontext.h>
#include <unicode/unum.h>
#include <unicode/ureldatefmt.h>
#include <unicode/unistr.h>

#include <memory>

extern "C" {
#include "../php_intl.h"
#include "../intl_data.h"
#include "../intl_error.h"
}

#include "../intl_convertcpp.h"
#include "reldateformatter_class.h"
#include "reldateformatter_arginfo.h"

using icu::Locale;
using icu::UnicodeString;

zend_class_entry *IntlRelativeDateTimeFormatter_ce_ptr;
static zend_object_handlers reldateformatter_handlers;

static void reldateformatter_free_object(zend_object *object)
{
	IntlRelativeDateTimeFormatter_object *obj = php_intl_reldateformatter_fetch_object(object);

	if (RELDATEFORMATTER_OBJECT(obj) != nullptr) {
		ureldatefmt_close(RELDATEFORMATTER_OBJECT(obj));
		RELDATEFORMATTER_OBJECT(obj) = nullptr;
	}
	intl_error_reset(RELDATEFORMATTER_ERROR_P(obj));

	zend_object_std_dtor(&obj->zo);
}

static zend_object *reldateformatter_create_object(zend_class_entry *class_type)
{
	IntlRelativeDateTimeFormatter_object *obj = reinterpret_cast<IntlRelativeDateTimeFormatter_object *>(
		zend_object_alloc(sizeof(IntlRelativeDateTimeFormatter_object), class_type));

	intl_error_init(RELDATEFORMATTER_ERROR_P(obj));
	RELDATEFORMATTER_OBJECT(obj) = nullptr;

	zend_object_std_init(&obj->zo, class_type);
	object_properties_init(&obj->zo, class_type);
	obj->zo.handlers = &reldateformatter_handlers;

	return &obj->zo;
}

static bool reldateformatter_valid_style(zend_long style)
{
	return style == UDAT_STYLE_LONG || style == UDAT_STYLE_SHORT || style == UDAT_STYLE_NARROW;
}

static bool reldateformatter_valid_capitalization_context(zend_long context)
{
	return context == UDISPCTX_CAPITALIZATION_NONE
		|| context == UDISPCTX_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE
		|| context == UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE
		|| context == UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU
		|| context == UDISPCTX_CAPITALIZATION_FOR_STANDALONE;
}

static bool reldateformatter_valid_unit(zend_long unit)
{
	return unit >= UDAT_REL_UNIT_YEAR && unit <= UDAT_REL_UNIT_SATURDAY;
}

static void reldateformatter_throw_constructor_failure(
	IntlRelativeDateTimeFormatter_object *obj,
	UErrorCode status,
	const char *message
)
{
	const bool old_use_exceptions = INTL_G(use_exceptions);
	const zend_long old_error_level = INTL_G(error_level);
	INTL_G(use_exceptions) = true;
	INTL_G(error_level) = 0;

	intl_errors_set(RELDATEFORMATTER_ERROR_P(obj), status, message);

	INTL_G(use_exceptions) = old_use_exceptions;
	INTL_G(error_level) = old_error_level;
}

PHP_METHOD(IntlRelativeDateTimeFormatter, __construct)
{
	IntlRelativeDateTimeFormatter_object *obj = Z_INTL_RELDATEFORMATTER_P(ZEND_THIS);
	char *locale = nullptr;
	size_t locale_len = 0;
	zend_long style = UDAT_STYLE_LONG;
	zend_long capitalization_context = UDISPCTX_CAPITALIZATION_NONE;
	zval *number_formatter = nullptr;

	ZEND_PARSE_PARAMETERS_START(0, 4)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(locale, locale_len)
		Z_PARAM_LONG(style)
		Z_PARAM_LONG(capitalization_context)
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(number_formatter, NumberFormatter_ce_ptr)
	ZEND_PARSE_PARAMETERS_END();

	intl_errors_reset(RELDATEFORMATTER_ERROR_P(obj));

	if (RELDATEFORMATTER_OBJECT(obj) != nullptr) {
		zend_throw_error(NULL, "IntlRelativeDateTimeFormatter object is already constructed");
		RETURN_THROWS();
	}

	if (UNEXPECTED(locale_len > INTL_MAX_LOCALE_LEN)) {
		zend_argument_value_error(1, "must be less than or equal to %d characters", INTL_MAX_LOCALE_LEN);
		RETURN_THROWS();
	}

	if (locale == nullptr || locale_len == 0) {
		locale = const_cast<char *>(intl_locale_get_default());
	}

	if (Locale(locale).getISO3Language()[0] == '\0') {
		zend_argument_value_error(1, "\"%s\" is invalid", locale);
		RETURN_THROWS();
	}

	if (!reldateformatter_valid_style(style)) {
		zend_argument_value_error(2,
			"must be one of IntlRelativeDateTimeFormatter::STYLE_LONG, "
			"IntlRelativeDateTimeFormatter::STYLE_SHORT, or "
			"IntlRelativeDateTimeFormatter::STYLE_NARROW");
		RETURN_THROWS();
	}

	if (!reldateformatter_valid_capitalization_context(capitalization_context)) {
		zend_argument_value_error(3,
			"must be one of the IntlRelativeDateTimeFormatter::CAPITALIZATION_* constants");
		RETURN_THROWS();
	}

	UErrorCode status = U_ZERO_ERROR;
	UNumberFormat *number_formatter_clone = nullptr;
	if (number_formatter != nullptr) {
		NumberFormatter_object *number_formatter_obj = Z_INTL_NUMBERFORMATTER_P(number_formatter);
		if (FORMATTER_OBJECT(number_formatter_obj) == nullptr) {
			zend_throw_error(NULL, "Found unconstructed NumberFormatter");
			RETURN_THROWS();
		}

		number_formatter_clone = unum_clone(
			reinterpret_cast<const UNumberFormat *>(FORMATTER_OBJECT(number_formatter_obj)),
			&status);
		if (U_FAILURE(status)) {
			reldateformatter_throw_constructor_failure(obj, status, "Failed to clone NumberFormatter");
			RETURN_THROWS();
		}
	}

	RELDATEFORMATTER_OBJECT(obj) = ureldatefmt_open(
		locale,
		number_formatter_clone,
		static_cast<UDateRelativeDateTimeFormatterStyle>(style),
		static_cast<UDisplayContext>(capitalization_context),
		&status);

	if (U_FAILURE(status) || RELDATEFORMATTER_OBJECT(obj) == nullptr) {
		if (U_SUCCESS(status)) {
			status = U_MEMORY_ALLOCATION_ERROR;
		}
		reldateformatter_throw_constructor_failure(
			obj, status, "Failed to create IntlRelativeDateTimeFormatter");
		RETURN_THROWS();
	}
}

template<typename Formatter>
static zend_string *reldateformatter_format_result(
	IntlRelativeDateTimeFormatter_object *obj,
	Formatter &&formatter,
	const char *failure_message
)
{
	UErrorCode status = U_ZERO_ERROR;
	int32_t result_len = formatter(nullptr, 0, &status);

	if (status == U_BUFFER_OVERFLOW_ERROR) {
		status = U_ZERO_ERROR;
	} else if (U_FAILURE(status)) {
		intl_errors_set(RELDATEFORMATTER_ERROR_P(obj), status, failure_message);
		return nullptr;
	}

	if (result_len == 0) {
		return ZSTR_EMPTY_ALLOC();
	}

	std::unique_ptr<UChar[]> result(new UChar[static_cast<size_t>(result_len) + 1]);
	const int32_t actual_len = formatter(result.get(), result_len + 1, &status);
	if (U_FAILURE(status)) {
		intl_errors_set(RELDATEFORMATTER_ERROR_P(obj), status, failure_message);
		return nullptr;
	}

	UnicodeString unicode_result(result.get(), actual_len);
	zend_string *utf8_result = intl_charFromString(unicode_result, &status);
	if (utf8_result == nullptr) {
		intl_errors_set(RELDATEFORMATTER_ERROR_P(obj), status, "Failed to convert result to UTF-8");
	}

	return utf8_result;
}

static void reldateformatter_format(INTERNAL_FUNCTION_PARAMETERS, bool numeric)
{
	zval *offset;
	zend_long unit;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_NUMBER(offset)
		Z_PARAM_LONG(unit)
	ZEND_PARSE_PARAMETERS_END();

	IntlRelativeDateTimeFormatter_object *obj = Z_INTL_RELDATEFORMATTER_P(ZEND_THIS);
	intl_errors_reset(RELDATEFORMATTER_ERROR_P(obj));

	if (RELDATEFORMATTER_OBJECT(obj) == nullptr) {
		zend_throw_error(NULL, "Found unconstructed IntlRelativeDateTimeFormatter");
		RETURN_THROWS();
	}

	if (!reldateformatter_valid_unit(unit)) {
		zend_argument_value_error(2,
			"must be one of the IntlRelativeDateTimeFormatter::UNIT_* constants");
		RETURN_THROWS();
	}

	const double numeric_offset = zval_get_double(offset);
	zend_string *result;
	if (numeric) {
		result = reldateformatter_format_result(obj,
			[obj, numeric_offset, unit](UChar *buffer, int32_t capacity, UErrorCode *status) {
				return ureldatefmt_formatNumeric(
					RELDATEFORMATTER_OBJECT(obj), numeric_offset,
					static_cast<URelativeDateTimeUnit>(unit), buffer, capacity, status);
			},
			"Failed to format relative date/time numerically");
	} else {
		result = reldateformatter_format_result(obj,
			[obj, numeric_offset, unit](UChar *buffer, int32_t capacity, UErrorCode *status) {
				return ureldatefmt_format(
					RELDATEFORMATTER_OBJECT(obj), numeric_offset,
					static_cast<URelativeDateTimeUnit>(unit), buffer, capacity, status);
			},
			"Failed to format relative date/time");
	}

	if (result == nullptr) {
		RETURN_FALSE;
	}
	RETURN_STR(result);
}

PHP_METHOD(IntlRelativeDateTimeFormatter, format)
{
	reldateformatter_format(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(IntlRelativeDateTimeFormatter, formatNumeric)
{
	reldateformatter_format(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(IntlRelativeDateTimeFormatter, combineDateAndTime)
{
	zend_string *relative_date;
	zend_string *time;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(relative_date)
		Z_PARAM_STR(time)
	ZEND_PARSE_PARAMETERS_END();

	IntlRelativeDateTimeFormatter_object *obj = Z_INTL_RELDATEFORMATTER_P(ZEND_THIS);
	intl_errors_reset(RELDATEFORMATTER_ERROR_P(obj));

	if (RELDATEFORMATTER_OBJECT(obj) == nullptr) {
		zend_throw_error(NULL, "Found unconstructed IntlRelativeDateTimeFormatter");
		RETURN_THROWS();
	}

	UErrorCode status = U_ZERO_ERROR;
	UnicodeString relative_date_utf16;
	UnicodeString time_utf16;
	intl_stringFromChar(
		relative_date_utf16, ZSTR_VAL(relative_date), ZSTR_LEN(relative_date), &status);
	if (U_FAILURE(status)) {
		intl_errors_set(RELDATEFORMATTER_ERROR_P(obj), status,
			"Failed to convert relative date to UTF-16");
		RETURN_FALSE;
	}

	intl_stringFromChar(time_utf16, ZSTR_VAL(time), ZSTR_LEN(time), &status);
	if (U_FAILURE(status)) {
		intl_errors_set(RELDATEFORMATTER_ERROR_P(obj), status,
			"Failed to convert time to UTF-16");
		RETURN_FALSE;
	}

	zend_string *result = reldateformatter_format_result(obj,
		[obj, &relative_date_utf16, &time_utf16](UChar *buffer, int32_t capacity, UErrorCode *format_status) {
			return ureldatefmt_combineDateAndTime(
				RELDATEFORMATTER_OBJECT(obj),
				relative_date_utf16.getBuffer(), relative_date_utf16.length(),
				time_utf16.getBuffer(), time_utf16.length(),
				buffer, capacity, format_status);
		},
		"Failed to combine relative date and time");

	if (result == nullptr) {
		RETURN_FALSE;
	}
	RETURN_STR(result);
}

PHP_METHOD(IntlRelativeDateTimeFormatter, getErrorCode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IntlRelativeDateTimeFormatter_object *obj = Z_INTL_RELDATEFORMATTER_P(ZEND_THIS);
	RETURN_LONG(intl_error_get_code(RELDATEFORMATTER_ERROR_P(obj)));
}

PHP_METHOD(IntlRelativeDateTimeFormatter, getErrorMessage)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IntlRelativeDateTimeFormatter_object *obj = Z_INTL_RELDATEFORMATTER_P(ZEND_THIS);
	RETURN_STR(intl_error_get_message(RELDATEFORMATTER_ERROR_P(obj)));
}

void reldateformatter_register_class(void)
{
	IntlRelativeDateTimeFormatter_ce_ptr = register_class_IntlRelativeDateTimeFormatter();
	IntlRelativeDateTimeFormatter_ce_ptr->create_object = reldateformatter_create_object;
	IntlRelativeDateTimeFormatter_ce_ptr->default_object_handlers = &reldateformatter_handlers;

	memcpy(&reldateformatter_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	reldateformatter_handlers.offset = offsetof(IntlRelativeDateTimeFormatter_object, zo);
	reldateformatter_handlers.free_obj = reldateformatter_free_object;
	reldateformatter_handlers.clone_obj = nullptr;
}
