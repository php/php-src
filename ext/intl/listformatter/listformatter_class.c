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

/* {{{ listformatter_free_obj */
static void listformatter_free_obj(zend_object *object)
{
    ListFormatter_object *obj = php_intl_listformatter_fetch_object(object);
    listformatter_data_free(&obj->lf_data);
    zend_object_std_dtor(&obj->zo);
}
/* }}} */

/* {{{ listformatter_create_object */
static zend_object *listformatter_create_object(zend_class_entry *class_type)
{
    ListFormatter_object *obj;
    obj = zend_object_alloc(sizeof(ListFormatter_object), class_type);
    listformatter_data_init(&obj->lf_data);
    zend_object_std_init(&obj->zo, class_type);
    object_properties_init(&obj->zo, class_type);
    obj->zo.handlers = &listformatter_handlers;
    return &obj->zo;
}
/* }}} */

/* {{{ listformatter_create_object */
PHP_METHOD(IntlListFormatter, __construct)
{
    ListFormatter_object *obj = Z_INTL_LISTFORMATTER_P(ZEND_THIS);
    zend_string *locale;
    zend_long type = ULISTFMT_TYPE_AND;
    zend_long width = ULISTFMT_WIDTH_WIDE;
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STR(locale)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(type)
        Z_PARAM_LONG(width)
    ZEND_PARSE_PARAMETERS_END();

    if (strlen(uloc_getISO3Language(ZSTR_VAL(locale))) == 0) {
		zend_argument_value_error(1, "\"%s\" is invalid", ZSTR_VAL(locale));
		RETURN_THROWS();
	}

    UErrorCode status = U_ZERO_ERROR;
    if (U_ICU_VERSION_MAJOR_NUM >= 67) {

        LISTFORMATTER_OBJECT(obj) = ulistfmt_openForType(ZSTR_VAL(locale), type, width, &status);
    } else {
        if (type != ULISTFMT_TYPE_AND) {
            zend_argument_value_error(2, "ICU 66 and below only support IntlListFormatter::TYPE_AND");
            RETURN_THROWS();
        }

        if (width != ULISTFMT_WIDTH_WIDE) {
            zend_argument_value_error(3, "ICU 66 and below only support IntlListFormatter::WIDTH_WIDE");
            RETURN_THROWS();
        }

        LISTFORMATTER_OBJECT(obj) = ulistfmt_open(ZSTR_VAL(locale), &status);
    }
    if (U_FAILURE(status)) {
        intl_error_set(NULL, status, "Constructor failed", 0);
        zend_throw_exception(IntlException_ce_ptr, "Constructor failed", 0);
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ listformatter_format */
PHP_METHOD(IntlListFormatter, format)
{
    ListFormatter_object *obj = Z_INTL_LISTFORMATTER_P(ZEND_THIS);
    zval *strings;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(strings)
    ZEND_PARSE_PARAMETERS_END();

    if (!LISTFORMATTER_OBJECT(obj)) {
        intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "ListFormatter not properly constructed", 0);
        RETURN_FALSE;
    }

    HashTable *ht = Z_ARRVAL_P(strings);
    uint32_t count = zend_array_count(ht);
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
        if (U_FAILURE(status)) {
            efree(items);
            efree(itemLengths);
            zend_string_release(str_val);
            intl_error_set(NULL, status, "Failed to convert string to UTF-16", 0);
            RETURN_FALSE;
        }
        
        items[i] = ustr;
        itemLengths[i] = ustr_len;
        zend_string_release(str_val);
        i++;
    } ZEND_HASH_FOREACH_END();

    UErrorCode status = U_ZERO_ERROR;
    int32_t resultLength;
    UChar *result = NULL;

    resultLength = ulistfmt_format(LISTFORMATTER_OBJECT(obj), items, itemLengths, count, NULL, 0, &status);

    if (U_FAILURE(status) && status != U_BUFFER_OVERFLOW_ERROR) {
        intl_error_set(NULL, status, "Failed to format list", 0);
        RETURN_FALSE;
    }

    // Allocate buffer and try again
    status = U_ZERO_ERROR;
    result = (UChar *)emalloc((resultLength + 1) * sizeof(UChar));
    ulistfmt_format(LISTFORMATTER_OBJECT(obj), items, itemLengths, count, result, resultLength, &status);

    // Clean up input strings
    for (i = 0; i < count; i++) {
        efree((void *)items[i]);
    }
    efree(items);
    efree(itemLengths);

    if (U_FAILURE(status)) {
        if (result) {
            efree(result);
        }
        intl_error_set(NULL, status, "Failed to format list", 0);
        RETURN_FALSE;
    }

    // Convert result back to UTF-8
    zend_string *ret = intl_convert_utf16_to_utf8(result, resultLength, &status);
    efree(result);
    
    if (!ret) {
        intl_error_set(NULL, status, "Failed to convert result to UTF-8", 0);
        RETURN_FALSE;
    }

    RETURN_STR(ret);
}
/* }}} */

/* {{{ listformatter_getErrorCode */
PHP_METHOD(IntlListFormatter, getErrorCode)
{
    ListFormatter_object *obj = Z_INTL_LISTFORMATTER_P(ZEND_THIS);

    UErrorCode status = intl_error_get_code(LISTFORMATTER_ERROR_P(obj));

    RETURN_LONG(status);
}
/* }}} */

/* {{{ listformatter_getErrorMessage */
PHP_METHOD(IntlListFormatter, getErrorMessage)
{
    ListFormatter_object *obj = Z_INTL_LISTFORMATTER_P(ZEND_THIS);

    zend_string *message = intl_error_get_message(LISTFORMATTER_ERROR_P(obj));
    RETURN_STR(message);
}
/* }}} */

/* {{{ listformatter_register_class */
void listformatter_register_class(void)
{
    zend_class_entry *class_entry = register_class_IntlListFormatter();
    class_entry->create_object = listformatter_create_object;
    
    memcpy(&listformatter_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    listformatter_handlers.offset = XtOffsetOf(ListFormatter_object, zo);
    listformatter_handlers.free_obj = listformatter_free_obj;
}
/* }}} */
