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
   | Authors: David Carlier <devnexen@gmail.com>                          |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "zend_API.h"
#include "../intl_common.h"

#if U_ICU_VERSION_MAJOR_NUM >= 63
#include <unicode/numberformatter.h>
#include <unicode/unistr.h>
#include <unicode/locid.h>
#include "../intl_convertcpp.h"
#include "../php_intl.h"

#include "numberformatter_arginfo.h"
#include "numberformatter_class.h"

using icu::number::NumberFormatter;
using icu::number::UnlocalizedNumberFormatter;
using icu::UnicodeString;

static zend_object_handlers numberformatter_handlers;
zend_class_entry *IntlNumberFormatter_ce_ptr;

static void numberformatter_free_obj(zend_object *object)
{
	IntlNumberFormatter_object *nfo = php_intl_numberformatter_fetch_object(object);

	delete NUMBERFORMATTER_OBJECT(nfo);
	NUMBERFORMATTER_OBJECT(nfo) = nullptr;
	intl_error_reset(INTL_DATA_ERROR_P(nfo));

	zend_object_std_dtor(&nfo->zo);
}

static zend_object *numberformatter_create_object(zend_class_entry *ce)
{
	IntlNumberFormatter_object *nfo = reinterpret_cast<IntlNumberFormatter_object *>(zend_object_alloc(sizeof(IntlNumberFormatter_object), ce));

	intl_error_init(INTL_DATA_ERROR_P(nfo));
	NUMBERFORMATTER_OBJECT(nfo) = nullptr;

	zend_object_std_init(&nfo->zo, ce);
	object_properties_init(&nfo->zo, ce);
	nfo->zo.handlers = &numberformatter_handlers;

	return &nfo->zo;
}

PHP_METHOD(IntlNumberFormatter, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_throw_error(nullptr, "Cannot directly construct %s, use createFromSkeleton method instead", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
}

PHP_METHOD(IntlNumberFormatter, createFromSkeleton)
{
	char *skeleton, *locale;
	size_t skeleton_len, locale_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(skeleton, skeleton_len)
		Z_PARAM_STRING(locale, locale_len)
	ZEND_PARSE_PARAMETERS_END();

	intl_error_reset(nullptr);

	INTL_CHECK_LOCALE_LEN(locale_len);

	if (locale_len == 0) {
		locale = (char *)intl_locale_get_default();
	}

	if (icu::Locale(locale).getISO3Language()[0] == '\0') {
		zend_argument_value_error(2, "\"%s\" is invalid", locale);
		RETURN_THROWS();
	}

	const char *scale = zend_memnstr(skeleton, "scale/", sizeof("scale/") - 1, skeleton + skeleton_len);

	if (scale) {
		scale += sizeof("scale/") - 1;
		scale += strspn(scale, "0123456789.+-");
		if (UNEXPECTED(*scale == 'e' || *scale == 'E')) {
			intl_error_set(nullptr, U_NUMBER_SKELETON_SYNTAX_ERROR, "Number formatter creation failed");
			RETURN_NULL();
		}
	}

	UErrorCode status = U_ZERO_ERROR;
	UnicodeString uskeleton;

	intl_stringFromChar(uskeleton, skeleton, skeleton_len, &status);
	INTL_CHECK_STATUS_OR_NULL(status, "Error converting skeleton to UTF-16");

	UnlocalizedNumberFormatter nf = NumberFormatter::forSkeleton(uskeleton, status);
	INTL_CHECK_STATUS_OR_NULL(status, "Number formatter creation failed");

	object_init_ex(return_value, IntlNumberFormatter_ce_ptr);
	NUMBERFORMATTER_OBJECT(Z_INTL_INTLNUMBERFORMATTER_P(return_value)) = new LocalizedNumberFormatter(nf.locale(icu::Locale::createCanonical(locale)));
}

PHP_METHOD(IntlNumberFormatter, format)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_NUMBER_OR_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	IntlNumberFormatter_object *nfo = Z_INTL_INTLNUMBERFORMATTER_P(ZEND_THIS);
	intl_errors_reset(INTL_DATA_ERROR_P(nfo));

	UErrorCode status = U_ZERO_ERROR;
	UnicodeString result;

	if (Z_TYPE_P(value) == IS_STRING) {
		if (UNEXPECTED(zend_str_has_nul_byte(Z_STR_P(value)))) {
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}
		if (UNEXPECTED(Z_STRLEN_P(value) > INT32_MAX)) {
			zend_argument_value_error(1, "is too long");
			RETURN_THROWS();
		}
		if (UNEXPECTED(strpbrk(Z_STRVAL_P(value), "eE"))) {
			intl_errors_set(INTL_DATA_ERROR_P(nfo), U_DECIMAL_NUMBER_SYNTAX_ERROR, "Number formatting failed");
			RETURN_FALSE;
		}
		result = NUMBERFORMATTER_OBJECT(nfo)->formatDecimal(icu::StringPiece(Z_STRVAL_P(value), static_cast<int32_t>(Z_STRLEN_P(value))), status).toString(status);
	} else if (Z_TYPE_P(value) == IS_DOUBLE) {
		result = NUMBERFORMATTER_OBJECT(nfo)->formatDouble(Z_DVAL_P(value), status).toString(status);
	} else {
		result = NUMBERFORMATTER_OBJECT(nfo)->formatInt(static_cast<int64_t>(Z_LVAL_P(value)), status).toString(status);
	}

	if (U_FAILURE(status)) {
		intl_errors_set(INTL_DATA_ERROR_P(nfo), status, "Number formatting failed");
		RETURN_FALSE;
	}

	zend_string *ret = intl_charFromString(result, &status);

	if (!ret) {
		intl_errors_set(INTL_DATA_ERROR_P(nfo), status, "Error converting value to UTF-8");
		RETURN_FALSE;
	}

	RETVAL_STR(ret);
}

PHP_METHOD(IntlNumberFormatter, getErrorCode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(INTL_DATA_ERROR_CODE(Z_INTL_INTLNUMBERFORMATTER_P(ZEND_THIS)));
}

PHP_METHOD(IntlNumberFormatter, getErrorMessage)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STR(intl_error_get_message(INTL_DATA_ERROR_P(Z_INTL_INTLNUMBERFORMATTER_P(ZEND_THIS))));
}

void numberformatter_register_class(void)
{
	IntlNumberFormatter_ce_ptr = register_class_IntlNumberFormatter();
	IntlNumberFormatter_ce_ptr->create_object = numberformatter_create_object;

	memcpy(&numberformatter_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	numberformatter_handlers.offset = offsetof(IntlNumberFormatter_object, zo);
	numberformatter_handlers.free_obj = numberformatter_free_obj;
	numberformatter_handlers.clone_obj = nullptr;
}
#endif
