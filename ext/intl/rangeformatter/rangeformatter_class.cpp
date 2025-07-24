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

#include <unicode/numberrangeformatter.h>
#include <unicode/unumberrangeformatter.h>
#include <unicode/numberformatter.h>
#include <unicode/unistr.h>
#include "../intl_convertcpp.h"

extern "C" {
    #include "php.h"
    #include "../php_intl.h"
    #include "../intl_data.h"
    #include "rangeformatter_arginfo.h"
    #include "rangeformatter_class.h"
#include "intl_convert.h"
}

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

	intern = (IntlNumberRangeFormatter_object*)zend_object_alloc(sizeof(IntlNumberRangeFormatter_object), ce);
	zend_object_std_init(&intern->zo, ce);
	object_properties_init(&intern->zo, ce);

	return &intern->zo;
}

U_CFUNC PHP_METHOD(IntlNumberRangeFormatter, __construct)
{
   
}

U_CFUNC PHP_METHOD(IntlNumberRangeFormatter, createFromSkeleton)
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

    if (skeleton_len == 0) {
        zend_argument_value_error(1, "Skeleton string cannot be empty");
        RETURN_THROWS();
    }

    if (locale_len > INTL_MAX_LOCALE_LEN) {
        zend_argument_value_error(2, "Locale string too long, should be no longer than %d characters", INTL_MAX_LOCALE_LEN);
        RETURN_THROWS();
    }

    if (strlen(uloc_getISO3Language(locale)) == 0) {
        zend_argument_value_error(2, "\"%s\" is invalid", locale);
        RETURN_THROWS();
    }

    if (collapse != UNUM_RANGE_COLLAPSE_AUTO && collapse != UNUM_RANGE_COLLAPSE_NONE && collapse != UNUM_RANGE_COLLAPSE_UNIT && collapse != UNUM_RANGE_COLLAPSE_ALL) {
        zend_argument_value_error(3, "Invalid collapse value");
        RETURN_THROWS();
    }

    if (identityFallback != UNUM_IDENTITY_FALLBACK_SINGLE_VALUE && identityFallback != UNUM_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE && identityFallback != UNUM_IDENTITY_FALLBACK_APPROXIMATELY && identityFallback != UNUM_IDENTITY_FALLBACK_RANGE) {
        zend_argument_value_error(4, "Invalid identity fallback value");
        RETURN_THROWS();
    }

    UErrorCode error = U_ZERO_ERROR;

    UnicodeString skeleton_ustr(skeleton, skeleton_len);

    UnlocalizedNumberFormatter nf = NumberFormatter::forSkeleton(skeleton_ustr, error);

    LocalizedNumberRangeFormatter* nrf = new LocalizedNumberRangeFormatter(
        NumberRangeFormatter::with()
            .locale(locale)
            .numberFormatterBoth(nf)
            .collapse(static_cast<UNumberRangeCollapse>(collapse))
            .identityFallback(static_cast<UNumberRangeIdentityFallback>(identityFallback))
    );

    zend_object* obj = IntlNumberRangeFormatter_object_create(class_entry_IntlNumberRangeFormatter);
    obj->handlers = &rangeformatter_handlers;

    RANGEFORMATTER_OBJECT(php_intl_numberrangeformatter_fetch_object(obj)) = nrf;

    RETURN_OBJ(obj);
    
}

U_CFUNC PHP_METHOD(IntlNumberRangeFormatter, format)
{
    double start;
    double end;

    IntlNumberRangeFormatter_object* obj = Z_INTL_RANGEFORMATTER_P(ZEND_THIS);

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_DOUBLE(start)
        Z_PARAM_DOUBLE(end)
    ZEND_PARSE_PARAMETERS_END();

    UErrorCode error = U_ZERO_ERROR;

    UnicodeString result = RANGEFORMATTER_OBJECT(obj)->formatFormattableRange(start, end, error).toString(error);

    if (U_FAILURE(error)) {
        intl_error_set(NULL, error, "Failed to format number range", 0);

        return;
    }

   zend_string *ret = intl_charFromString(result, &error);

   if (!ret) {
        intl_error_set(NULL, error, "Failed to convert result to UTF-8", 0);
        // RETVAL_FALSE;
        return;
   }

   RETVAL_NEW_STR(ret);
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
