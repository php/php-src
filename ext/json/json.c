/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Omar Kilani <omar@php.net>                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"
#include "utf8_to_utf16.h"
#include "JSON_parser.h"
#include "php_json.h"

static PHP_MINFO_FUNCTION(json);

static PHP_FUNCTION(json_encode);
static PHP_FUNCTION(json_decode);

static const char digits[] = "0123456789abcdef";

/* {{{ json_functions[]
 *
 * Every user visible function must have an entry in json_functions[].
 */
static function_entry json_functions[] = {
    PHP_FE(json_encode, NULL)
    PHP_FE(json_decode, NULL)
    {NULL, NULL, NULL}  /* Must be the last line in json_functions[] */
};
/* }}} */

/* {{{ json_module_entry
 */
zend_module_entry json_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "json",
    json_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_MINFO(json),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_JSON_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_JSON
ZEND_GET_MODULE(json)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(json)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "json support", "enabled");
    php_info_print_table_row(2, "json version", PHP_JSON_VERSION);
    php_info_print_table_end();
}
/* }}} */

static void json_encode_r(smart_str *buf, zval *val TSRMLS_DC);
static void json_escape_string(smart_str *buf, char *s, int len TSRMLS_DC);

static int json_determine_array_type(zval **val TSRMLS_DC)  /* {{{ */
{
    int i;
    HashTable *myht = HASH_OF(*val);

    i = myht ? zend_hash_num_elements(myht) : 0;
    if (i > 0) {
        char *key;
        ulong index, idx;
        uint key_len;
        HashPosition pos;

        zend_hash_internal_pointer_reset_ex(myht, &pos);
        idx = 0;
        for (;; zend_hash_move_forward_ex(myht, &pos)) {
            i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
            if (i == HASH_KEY_NON_EXISTANT)
                break;

            if (i == HASH_KEY_IS_STRING) {
                return 1;
            } else {
                if (index != idx) {
                    return 1;
                }
            }
            idx++;
        }
    }

    return 0;
}
/* }}} */

static void json_encode_array(smart_str *buf, zval **val TSRMLS_DC) { /* {{{ */
    int i, r;
    HashTable *myht;

    if (Z_TYPE_PP(val) == IS_ARRAY) {
        myht = HASH_OF(*val);
        r = json_determine_array_type(val TSRMLS_CC);
    } else {
        myht = Z_OBJPROP_PP(val);
        r = 1;
    }

    if (myht && myht->nApplyCount > 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");
        smart_str_appendl(buf, "null", 4);
        return;
    }

    if (r == 0)
    {
        smart_str_appendc(buf, '[');
    }
    else
    {
        smart_str_appendc(buf, '{');
    }

    i = myht ? zend_hash_num_elements(myht) : 0;
    if (i > 0) {
        char *key;
        zval **data;
        ulong index;
        uint key_len;
        HashPosition pos;
        HashTable *tmp_ht;
        int need_comma = 0;

        zend_hash_internal_pointer_reset_ex(myht, &pos);
        for (;; zend_hash_move_forward_ex(myht, &pos)) {
            i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
            if (i == HASH_KEY_NON_EXISTANT)
                break;

            if (zend_hash_get_current_data_ex(myht, (void **) &data, &pos) == SUCCESS) {
                tmp_ht = HASH_OF(*data);
                if (tmp_ht) {
                    tmp_ht->nApplyCount++;
                }

                if (r == 0) {
                    if (need_comma) {
                        smart_str_appendc(buf, ',');
                    } else {
                        need_comma = 1;
                    }
 
                    json_encode_r(buf, *data TSRMLS_CC);
                } else if (r == 1) {
                    if (i == HASH_KEY_IS_STRING) {
                        if (key[0] == '\0' && Z_TYPE_PP(val) == IS_OBJECT) {
                            /* Skip protected and private members. */
							if (tmp_ht) {
								tmp_ht->nApplyCount--;
							}
                            continue;
                        }

                        if (need_comma) {
                            smart_str_appendc(buf, ',');
                        } else {
                            need_comma = 1;
                        }

                        json_escape_string(buf, key, key_len - 1 TSRMLS_CC);
                        smart_str_appendc(buf, ':');

                        json_encode_r(buf, *data TSRMLS_CC);
                    } else {
                        if (need_comma) {
                            smart_str_appendc(buf, ',');
                        } else {
                            need_comma = 1;
                        }
                        
                        smart_str_appendc(buf, '"');
                        smart_str_append_long(buf, (long) index);
                        smart_str_appendc(buf, '"');
                        smart_str_appendc(buf, ':');

                        json_encode_r(buf, *data TSRMLS_CC);
                    }
                }

                if (tmp_ht) {
                    tmp_ht->nApplyCount--;
                }
            }
        }
    }

    if (r == 0)
    {
        smart_str_appendc(buf, ']');
    }
    else
    {
        smart_str_appendc(buf, '}');
    }
}
/* }}} */

#define REVERSE16(us) (((us & 0xf) << 12) | (((us >> 4) & 0xf) << 8) | (((us >> 8) & 0xf) << 4) | ((us >> 12) & 0xf))

static void json_escape_string(smart_str *buf, char *s, int len TSRMLS_DC) /* {{{ */
{
    int pos = 0;
    unsigned short us;
    unsigned short *utf16;

    if (len == 0)
    {
        smart_str_appendl(buf, "\"\"", 2);
        return;
    }

    utf16 = (unsigned short *) safe_emalloc(len, sizeof(unsigned short), 0);

    len = utf8_to_utf16(utf16, s, len);
    if (len <= 0)
    {
        if (utf16)
        {
            efree(utf16);
        }
	if(len < 0) {
		if(!PG(display_errors)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid UTF-8 sequence in argument");
		}
	        smart_str_appendl(buf, "null", 4);
	} else {
	        smart_str_appendl(buf, "\"\"", 2);
	}
        return;
    }

    smart_str_appendc(buf, '"');

    while(pos < len)
    {
        us = utf16[pos++];

        switch (us)
        {
            case '"':
                {
                    smart_str_appendl(buf, "\\\"", 2);
                }
                break;
            case '\\':
                {
                    smart_str_appendl(buf, "\\\\", 2);
                }
                break;
            case '/':
                {
                    smart_str_appendl(buf, "\\/", 2);
                }
                break;
            case '\b':
                {
                    smart_str_appendl(buf, "\\b", 2);
                }
                break;
            case '\f':
                {
                    smart_str_appendl(buf, "\\f", 2);
                }
                break;
            case '\n':
                {
                    smart_str_appendl(buf, "\\n", 2);
                }
                break;
            case '\r':
                {
                    smart_str_appendl(buf, "\\r", 2);
                }
                break;
            case '\t':
                {
                    smart_str_appendl(buf, "\\t", 2);
                }
                break;
            default:
                {
                    if (us >= ' ' && (us & 127) == us)
                    {
                        smart_str_appendc(buf, (unsigned char) us);
                    }
                    else
                    {
                        smart_str_appendl(buf, "\\u", 2);
                        us = REVERSE16(us);

                        smart_str_appendc(buf, digits[us & ((1 << 4) - 1)]);
                        us >>= 4;
                        smart_str_appendc(buf, digits[us & ((1 << 4) - 1)]);
                        us >>= 4;
                        smart_str_appendc(buf, digits[us & ((1 << 4) - 1)]);
                        us >>= 4;
                        smart_str_appendc(buf, digits[us & ((1 << 4) - 1)]);
                    }
                }
                break;
        }
    }

    smart_str_appendc(buf, '"');
    efree(utf16);
}
/* }}} */

static void json_encode_r(smart_str *buf, zval *val TSRMLS_DC) /* {{{ */
{
    switch (Z_TYPE_P(val)) {
        case IS_NULL:
            smart_str_appendl(buf, "null", 4);
            break;
        case IS_BOOL:
            if (Z_BVAL_P(val))
            {
                smart_str_appendl(buf, "true", 4);
            }
            else
            {
                smart_str_appendl(buf, "false", 5);
            }
            break;
        case IS_LONG:
	    smart_str_append_long(buf, Z_LVAL_P(val));
            break;
        case IS_DOUBLE:
            {
                char *d = NULL;
                int len;
                double dbl = Z_DVAL_P(val);

                if (!zend_isinf(dbl) && !zend_isnan(dbl)) {
			len = spprintf(&d, 0, "%.*k", (int) EG(precision), dbl);
			smart_str_appendl(buf, d, len);
			efree(d);
                } else {
                    zend_error(E_WARNING, "[json] (json_encode_r) double %.9g does not conform to the JSON spec, encoded as 0.", dbl);
                    smart_str_appendc(buf, '0');
                }
            }
            break;
        case IS_STRING:
            json_escape_string(buf, Z_STRVAL_P(val), Z_STRLEN_P(val) TSRMLS_CC);
            break;
        case IS_ARRAY:
        case IS_OBJECT:
            json_encode_array(buf, &val TSRMLS_CC);
            break;
        default:
            zend_error(E_WARNING, "[json] (json_encode_r) type is unsupported, encoded as null.");
            smart_str_appendl(buf, "null", 4);
            break;
    }

    return;
}
/* }}} */

/* {{{ proto string json_encode(mixed data)
   Returns the JSON representation of a value */
static PHP_FUNCTION(json_encode)
{
    zval *parameter;
    smart_str buf = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &parameter) == FAILURE) {
        return;
    }

    json_encode_r(&buf, parameter TSRMLS_CC);

    ZVAL_STRINGL(return_value, buf.c, buf.len, 1);

    smart_str_free(&buf);
}
/* }}} */

/* {{{ proto mixed json_decode(string json [, bool assoc])
   Decodes the JSON representation into a PHP value */
static PHP_FUNCTION(json_decode)
{
    char *parameter;
    int parameter_len, utf16_len;
    zend_bool assoc = 0; /* return JS objects as PHP objects by default */
    zval *z;
    unsigned short *utf16;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &parameter, &parameter_len, &assoc) == FAILURE) {
        return;
    }

    if (!parameter_len)
    {
        RETURN_NULL();
    }

    utf16 = (unsigned short *) safe_emalloc((parameter_len+1), sizeof(unsigned short), 1);

    utf16_len = utf8_to_utf16(utf16, parameter, parameter_len);
    if (utf16_len <= 0)
    {
        if (utf16)
        {
            efree(utf16);
        }

        RETURN_NULL();
    }

    ALLOC_INIT_ZVAL(z);
    if (JSON_parser(z, utf16, utf16_len, assoc TSRMLS_CC))
    {
        *return_value = *z;

        FREE_ZVAL(z);
        efree(utf16);
    }
    else
    {
	double d;
	int type;
	long p;

        zval_dtor(z);
        FREE_ZVAL(z);
        efree(utf16);

	if (parameter_len == 4) {
		if (!strcasecmp(parameter, "null")) {
			RETURN_NULL();
		} else if (!strcasecmp(parameter, "true")) {
			RETURN_BOOL(1);
		}
	} else if (parameter_len == 5 && !strcasecmp(parameter, "false")) {
		RETURN_BOOL(0);
	}
	if ((type = is_numeric_string(parameter, parameter_len, &p, &d, 0)) != 0) {
		if (type == IS_LONG) {
			RETURN_LONG(p);
		} else if (type == IS_DOUBLE) {
			RETURN_DOUBLE(d);
		}
	}
		RETURN_NULL();
    }
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
