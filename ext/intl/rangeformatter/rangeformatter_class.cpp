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
   | Authors: Bogdan Ungureanu <bogdanungureanu21@gmail.com>              |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "zend_API.h"
#include "../intl_common.h"

#if U_ICU_VERSION_MAJOR_NUM >= 63
#include <unicode/numberrangeformatter.h>
#include <unicode/numberformatter.h>
#include <unicode/unistr.h>
#include "../intl_convert.h"

#include "../intl_error.h"
#include "../php_intl.h"
#include "../intl_data.h"
#include "rangeformatter_arginfo.h"
#include "rangeformatter_class.h"

using icu::number::NumberRangeFormatter;
using icu::number::NumberFormatter;
using icu::number::UnlocalizedNumberFormatter;
using icu::number::LocalizedNumberRangeFormatter;
using icu::UnicodeString;
using icu::MeasureUnit;

static zend_object_handlers rangeformatter_handlers;
zend_class_entry *class_entry_IntlNumberRangeFormatter;

zend_object *IntlNumberRangeFormatter_object_create(zend_class_entry *ce)
{
    IntlNumberRangeFormatter_object* intern;

    intern = reinterpret_cast<IntlNumberRangeFormatter_object*>(zend_object_alloc(sizeof(IntlNumberRangeFormatter_object), ce));
    zend_object_std_init(&intern->zo, ce);
    object_properties_init(&intern->zo, ce);

    // Initialize rangeformatter_data structure
    intl_error_init(&intern->nrf_data.error);
    intern->nrf_data.unumrf = nullptr;

    intern->zo.handlers = &rangeformatter_handlers;

    return &intern->zo;
}

PHP_METHOD(IntlNumberRangeFormatter, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();
    zend_throw_error(NULL, "Cannot directly construct %s, use createFromSkeleton method instead", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
}

PHP_METHOD(IntlNumberRangeFormatter, createFromSkeleton)
{
    char* skeleton;
    char* locale;
    size_t locale_len;
    size_t skeleton_len;
    zend_long collapse;
    zend_long identityFallback;

    ZEND_PARSE_PARAMETERS_START(4,4)
        Z_PARAM_STRING(skeleton, skeleton_len)
        Z_PARAM_STRING(locale, locale_len)
        Z_PARAM_LONG(collapse)
        Z_PARAM_LONG(identityFallback)
    ZEND_PARSE_PARAMETERS_END();

    if (locale_len == 0) {
        locale = (char *)intl_locale_get_default();
    }

    if (locale_len > INTL_MAX_LOCALE_LEN) {
        zend_argument_value_error(2, "must be no longer than %d characters", INTL_MAX_LOCALE_LEN);
        RETURN_THROWS();
    }

    if (strlen(uloc_getISO3Language(locale)) == 0) {
        zend_argument_value_error(2, "\"%s\" is invalid", locale);
        RETURN_THROWS();
    }

    if (collapse != UNUM_RANGE_COLLAPSE_AUTO && collapse != UNUM_RANGE_COLLAPSE_NONE && collapse != UNUM_RANGE_COLLAPSE_UNIT && collapse != UNUM_RANGE_COLLAPSE_ALL) {
        zend_argument_value_error(3, "must be one of IntlNumberRangeFormatter::COLLAPSE_AUTO, IntlNumberRangeFormatter::COLLAPSE_NONE, IntlNumberRangeFormatter::COLLAPSE_UNIT, or IntlNumberRangeFormatter::COLLAPSE_ALL");
        RETURN_THROWS();
    }

    if (identityFallback != UNUM_IDENTITY_FALLBACK_SINGLE_VALUE && identityFallback != UNUM_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE && identityFallback != UNUM_IDENTITY_FALLBACK_APPROXIMATELY && identityFallback != UNUM_IDENTITY_FALLBACK_RANGE) {
        zend_argument_value_error(4, "must be one of IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE, IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE, IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY, or IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE");
        RETURN_THROWS();
    }

    UErrorCode status = U_ZERO_ERROR;

    UnicodeString skeleton_ustr(skeleton, skeleton_len);

    UnlocalizedNumberFormatter nf = NumberFormatter::forSkeleton(skeleton_ustr, status);

    if (U_FAILURE(status)) {
        // override error level and use exceptions
        const bool old_use_exception = INTL_G(use_exceptions);
        const zend_long old_error_level = INTL_G(error_level);
        INTL_G(use_exceptions) = true;
        INTL_G(error_level) = 0;

        intl_error_set(NULL, status, "Failed to create the number skeleton");

        INTL_G(use_exceptions) = old_use_exception;
        INTL_G(error_level) = old_error_level;
    }

    LocalizedNumberRangeFormatter* nrf = new LocalizedNumberRangeFormatter(
        NumberRangeFormatter::with()
            .locale(locale)
            .numberFormatterBoth(nf)
            .collapse(static_cast<UNumberRangeCollapse>(collapse))
            .identityFallback(static_cast<UNumberRangeIdentityFallback>(identityFallback))
    );

    zend_object* obj = IntlNumberRangeFormatter_object_create(class_entry_IntlNumberRangeFormatter);

    RANGEFORMATTER_OBJECT(php_intl_numberrangeformatter_fetch_object(obj)) = nrf;

    RETURN_OBJ(obj);
}

PHP_METHOD(IntlNumberRangeFormatter, format)
{
    zval *start;
    zval *end;

    IntlNumberRangeFormatter_object* obj = Z_INTL_RANGEFORMATTER_P(ZEND_THIS);

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_NUMBER(start)
        Z_PARAM_NUMBER(end)
    ZEND_PARSE_PARAMETERS_END();

    UErrorCode error = U_ZERO_ERROR;

    icu::Formattable start_formattable(Z_TYPE_P(start) == IS_DOUBLE ? Z_DVAL_P(start) : Z_LVAL_P(start));
    icu::Formattable end_formattable(Z_TYPE_P(end) == IS_DOUBLE ? Z_DVAL_P(end) : Z_LVAL_P(end));

    UnicodeString result = RANGEFORMATTER_OBJECT(obj)->formatFormattableRange(start_formattable, end_formattable, error).toString(error);

    // override error level and use exceptions
    const bool old_use_exception = INTL_G(use_exceptions);
    const zend_long old_error_level = INTL_G(error_level);
    INTL_G(use_exceptions) = true;
    INTL_G(error_level) = 0;

    if (U_FAILURE(error)) {
        intl_error_set(NULL, error, "Failed to format number range");
    }

    zend_string *ret = intl_charFromString(result, &error);

    if (U_FAILURE(error)) {
        intl_error_set(NULL, error, "Failed to convert result to UTF-8");
    }

    INTL_G(use_exceptions) = old_use_exception;
    INTL_G(error_level) = old_error_level;

    RETVAL_NEW_STR(ret);
}

PHP_METHOD(IntlNumberRangeFormatter, getErrorCode)
{
    ZEND_PARSE_PARAMETERS_NONE();

    IntlNumberRangeFormatter_object* obj = Z_INTL_RANGEFORMATTER_P(ZEND_THIS);

    RETURN_LONG(intl_error_get_code(&obj->nrf_data.error));
}

PHP_METHOD(IntlNumberRangeFormatter, getErrorMessage)
{
    ZEND_PARSE_PARAMETERS_NONE();

    IntlNumberRangeFormatter_object* obj = Z_INTL_RANGEFORMATTER_P(ZEND_THIS);

    RETURN_STR(intl_error_get_message(&obj->nrf_data.error));
}

void IntlNumberRangeFormatter_object_free(zend_object *object)
{
    IntlNumberRangeFormatter_object* nfo = php_intl_numberrangeformatter_fetch_object(object);

    if (nfo->nrf_data.unumrf) {
        delete nfo->nrf_data.unumrf;
        nfo->nrf_data.unumrf = nullptr;
    }

    intl_error_reset(&nfo->nrf_data.error);

    zend_object_std_dtor(&nfo->zo);
}

void rangeformatter_register_class(void)
{
    class_entry_IntlNumberRangeFormatter = register_class_IntlNumberRangeFormatter();
    class_entry_IntlNumberRangeFormatter->create_object = IntlNumberRangeFormatter_object_create;

    memcpy(&rangeformatter_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    rangeformatter_handlers.offset = XtOffsetOf(IntlNumberRangeFormatter_object, zo);
    rangeformatter_handlers.free_obj = IntlNumberRangeFormatter_object_free;
    rangeformatter_handlers.clone_obj = NULL;
}
#endif
