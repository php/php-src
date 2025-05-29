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
#include "php_intl.h"
#include <unicode/ulistformatter.h>
#include "listformatter_arginfo.h"
#include "listformatter_class.h"
#include "intl_convert.h"

static zend_object_handlers listformatter_handlers;

static void listformatter_free_obj(zend_object *object)
{
    ListFormatter_object *obj = php_intl_listformatter_fetch_object(object);

    if( obj->lf_data.ulistfmt )
        ulistfmt_close( obj->lf_data.ulistfmt );

    obj->lf_data.ulistfmt = NULL;
    intl_error_reset( &obj->lf_data.error );

    zend_object_std_dtor(&obj->zo);
}

static zend_object *listformatter_create_object(zend_class_entry *class_type)
{
    ListFormatter_object *obj;
    obj = zend_object_alloc(sizeof(ListFormatter_object), class_type);

    obj->lf_data.ulistfmt = NULL;
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
    zend_long type = ULISTFMT_TYPE_AND;
    zend_long width = ULISTFMT_WIDTH_WIDE;
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
        zend_argument_value_error(1, "Locale string too long, should be no longer than %d characters", INTL_MAX_LOCALE_LEN);
        RETURN_THROWS();
    }

    if (strlen(uloc_getISO3Language(locale)) == 0) {
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

        LISTFORMATTER_OBJECT(obj) = ulistfmt_openForType(locale, type, width, &status);
    #else
        if (type != ULISTFMT_TYPE_AND) {
            zend_argument_value_error(2, "contains an unsupported type. ICU 66 and below only support IntlListFormatter::TYPE_AND");
            RETURN_THROWS();
        }

        if (width != ULISTFMT_WIDTH_WIDE) {
            zend_argument_value_error(3, "contains an unsupported width. ICU 66 and below only support IntlListFormatter::WIDTH_WIDE");
            RETURN_THROWS();
        }

        LISTFORMATTER_OBJECT(obj) = ulistfmt_open(locale, &status);
    #endif

    if (U_FAILURE(status)) {
        intl_error_set(NULL, status, "Constructor failed", 0);
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

    const UChar **items = (const UChar **)safe_emalloc(count, sizeof(const UChar *), 0);
    int32_t *itemLengths = (int32_t *)safe_emalloc(count, sizeof(int32_t), 0);
    uint32_t i = 0;
    zval *val;

    ZEND_HASH_FOREACH_VAL(ht, val) {
        zend_string *str_val;
        
        str_val = zval_get_string(val);
        
        // Convert PHP string to UTF-16
        UChar *ustr = NULL;
        int32_t ustr_len = 0;
        UErrorCode status = U_ZERO_ERROR;
        
        intl_convert_utf8_to_utf16(&ustr, &ustr_len, ZSTR_VAL(str_val), ZSTR_LEN(str_val), &status);
        zend_string_release(str_val);

        if (U_FAILURE(status)) {
            // We can't use goto cleanup because items and itemLengths are incompletely allocated
            for (uint32_t j = 0; j < i; j++) {
                efree((void *)items[j]);
            }
            efree(items);
            efree(itemLengths);
            intl_error_set(NULL, status, "Failed to convert string to UTF-16", 0);
            RETURN_FALSE;
        }
        
        items[i] = ustr;
        itemLengths[i] = ustr_len;
        i++;
    } ZEND_HASH_FOREACH_END();

    UErrorCode status = U_ZERO_ERROR;
    int32_t resultLength;
    UChar *result = NULL;

    resultLength = ulistfmt_format(LISTFORMATTER_OBJECT(obj), items, itemLengths, count, NULL, 0, &status);

    if (U_FAILURE(status) && status != U_BUFFER_OVERFLOW_ERROR) {
        intl_error_set(NULL, status, "Failed to format list", 0);
        RETVAL_FALSE;
        goto cleanup;
    }

    // Allocate buffer and try again
    status = U_ZERO_ERROR;
    result = (UChar *)safe_emalloc(resultLength + 1, sizeof(UChar), 0);
    ulistfmt_format(LISTFORMATTER_OBJECT(obj), items, itemLengths, count, result, resultLength, &status);

    if (U_FAILURE(status)) {
        if (result) {
            efree(result);
        }
        intl_error_set(NULL, status, "Failed to format list", 0);
        RETVAL_FALSE;
        goto cleanup;
    }

    // Convert result back to UTF-8
    zend_string *ret = intl_convert_utf16_to_utf8(result, resultLength, &status);
    efree(result);
    
    if (!ret) {
        intl_error_set(NULL, status, "Failed to convert result to UTF-8", 0);
        RETVAL_FALSE;
    } else {
        RETVAL_NEW_STR(ret);
    }

cleanup:
    for (i = 0; i < count; i++) {
        efree((void *)items[i]);
    }
    efree(items);
    efree(itemLengths);
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
    listformatter_handlers.clone_obj = NULL;
}
