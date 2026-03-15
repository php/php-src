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

extern "C" {
#include "php.h"
}

#include <unicode/listformatter.h>
#include <unicode/locid.h>
#include "../intl_convertcpp.h"

extern "C" {
#include "php_intl.h"
}
#include "listformatter_class.h"
#include "listformatter_arginfo.h"

static zend_object_handlers listformatter_handlers;

static void listformatter_free_obj(zend_object *object)
{
    ListFormatter_object *obj = php_intl_listformatter_fetch_object(object);

    delete obj->lf_data.ulistfmt;
    obj->lf_data.ulistfmt = nullptr;
    intl_error_reset( &obj->lf_data.error );

    zend_object_std_dtor(&obj->zo);
}

static zend_object *listformatter_create_object(zend_class_entry *class_type)
{
    ListFormatter_object *obj;
    obj = reinterpret_cast<ListFormatter_object *>(zend_object_alloc(sizeof(ListFormatter_object), class_type));

    obj->lf_data.ulistfmt = nullptr;
    intl_error_reset( &obj->lf_data.error );

    zend_object_std_init(&obj->zo, class_type);
    object_properties_init(&obj->zo, class_type);
    obj->zo.handlers = &listformatter_handlers;
    return &obj->zo;
}

PHP_METHOD(IntlListFormatter, __construct)
{
    ListFormatter_object *obj = Z_INTL_LISTFORMATTER_P(ZEND_THIS);
    char* locale;
    size_t locale_len = 0;
    #if U_ICU_VERSION_MAJOR_NUM >= 67
        zend_long type = ULISTFMT_TYPE_AND;
        zend_long width = ULISTFMT_WIDTH_WIDE;
    #else
        zend_long type = INTL_LISTFORMATTER_FALLBACK_TYPE_AND;
        zend_long width = INTL_LISTFORMATTER_FALLBACK_WIDTH_WIDE;
    #endif
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STRING(locale, locale_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(type)
        Z_PARAM_LONG(width)
    ZEND_PARSE_PARAMETERS_END();

    if(locale_len == 0) {
        locale = (char *)intl_locale_get_default();
    }

    if (locale_len > INTL_MAX_LOCALE_LEN) {
        zend_argument_value_error(1, "must be less than or equal to %d characters", INTL_MAX_LOCALE_LEN);
        RETURN_THROWS();
    }

    if (icu::Locale(locale).getISO3Language()[0] == '\0') {
        zend_argument_value_error(1, "\"%s\" is invalid", locale);
        RETURN_THROWS();
    }

    UErrorCode status = U_ZERO_ERROR;
    #if U_ICU_VERSION_MAJOR_NUM >= 67
        if (type != ULISTFMT_TYPE_AND && type != ULISTFMT_TYPE_OR && type != ULISTFMT_TYPE_UNITS) {
            zend_argument_value_error(2, "must be one of IntlListFormatter::TYPE_AND, IntlListFormatter::TYPE_OR, or IntlListFormatter::TYPE_UNITS");
            RETURN_THROWS();
        }

        if (width != ULISTFMT_WIDTH_WIDE && width != ULISTFMT_WIDTH_SHORT && width != ULISTFMT_WIDTH_NARROW) {
            zend_argument_value_error(3, "must be one of IntlListFormatter::WIDTH_WIDE, IntlListFormatter::WIDTH_SHORT, or IntlListFormatter::WIDTH_NARROW");
            RETURN_THROWS();
        }

        LISTFORMATTER_OBJECT(obj) = ListFormatter::createInstance(icu::Locale(locale), static_cast<UListFormatterType>(type), static_cast<UListFormatterWidth>(width), status);
    #else
        if (type != INTL_LISTFORMATTER_FALLBACK_TYPE_AND) {
            zend_argument_value_error(2, "contains an unsupported type. ICU 66 and below only support IntlListFormatter::TYPE_AND");
            RETURN_THROWS();
        }

        if (width != INTL_LISTFORMATTER_FALLBACK_WIDTH_WIDE) {
            zend_argument_value_error(3, "contains an unsupported width. ICU 66 and below only support IntlListFormatter::WIDTH_WIDE");
            RETURN_THROWS();
        }

        LISTFORMATTER_OBJECT(obj) = ListFormatter::createInstance(icu::Locale(locale), status);
    #endif

    if (U_FAILURE(status)) {
        intl_error_set(nullptr, status, "Constructor failed");
        zend_throw_exception(IntlException_ce_ptr, "Constructor failed", 0);
        RETURN_THROWS();
    }
}

PHP_METHOD(IntlListFormatter, format)
{
    ListFormatter_object *obj = Z_INTL_LISTFORMATTER_P(ZEND_THIS);
    HashTable *ht;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(ht)
    ZEND_PARSE_PARAMETERS_END();

    uint32_t count = zend_hash_num_elements(ht);
    if (count == 0) {
        RETURN_EMPTY_STRING();
    }

    UnicodeString *items = new UnicodeString[count];
    uint32_t i = 0;
    zval *val;

    ZEND_HASH_FOREACH_VAL(ht, val) {
        zend_string *str_val, *tmp_str;
        UErrorCode conv_status = U_ZERO_ERROR;

        str_val = zval_try_get_tmp_string(val, &tmp_str);
        if (UNEXPECTED(!str_val)) {
            delete[] items;
            RETURN_THROWS();
        }
        intl_stringFromChar(items[i], ZSTR_VAL(str_val), ZSTR_LEN(str_val), &conv_status);
        zend_tmp_string_release(tmp_str);

        if (U_FAILURE(conv_status)) {
            delete[] items;
            intl_error_set(nullptr, conv_status, "Failed to convert string to UTF-16");
            RETURN_FALSE;
        }

        i++;
    } ZEND_HASH_FOREACH_END();

    UErrorCode status = U_ZERO_ERROR;
    UnicodeString result;

    LISTFORMATTER_OBJECT(obj)->format(items, count, result, status);
    delete[] items;

    if (U_FAILURE(status)) {
        intl_error_set(nullptr, status, "Failed to format list");
        RETURN_FALSE;
    }

    zend_string *ret = intl_charFromString(result, &status);

    if (!ret) {
        intl_error_set(nullptr, status, "Failed to convert result to UTF-8");
        RETURN_FALSE;
    }

    RETVAL_STR(ret);
}

PHP_METHOD(IntlListFormatter, getErrorCode)
{
    ZEND_PARSE_PARAMETERS_NONE();

    ListFormatter_object *obj = Z_INTL_LISTFORMATTER_P(ZEND_THIS);

    UErrorCode status = intl_error_get_code(LISTFORMATTER_ERROR_P(obj));

    RETURN_LONG(status);
}

PHP_METHOD(IntlListFormatter, getErrorMessage)
{
     ZEND_PARSE_PARAMETERS_NONE();

    ListFormatter_object *obj = Z_INTL_LISTFORMATTER_P(ZEND_THIS);

    zend_string *message = intl_error_get_message(LISTFORMATTER_ERROR_P(obj));
    RETURN_STR(message);
}

void listformatter_register_class(void)
{
    zend_class_entry *class_entry = register_class_IntlListFormatter();
    class_entry->create_object = listformatter_create_object;

    memcpy(&listformatter_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    listformatter_handlers.offset = XtOffsetOf(ListFormatter_object, zo);
    listformatter_handlers.free_obj = listformatter_free_obj;
    listformatter_handlers.clone_obj = nullptr;
}
