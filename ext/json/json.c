/*
 * Copyright (c) 2005 Omar Kilani <omar@rmilk.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public (LGPL)
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details: http://www.gnu.org/
 *
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_json.h"
#include "json.h"

/* If you declare any globals in php_json.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(json)
*/

/* True global resources - no need for thread safety here */
static int le_json;

/* {{{ json_functions[]
 *
 * Every user visible function must have an entry in json_functions[].
 */
function_entry json_functions[] = {
	PHP_FE(json_encode,	NULL)
	PHP_FE(json_decode,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in json_functions[] */
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
PHP_MINFO_FUNCTION(json)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "json support", "enabled");
	php_info_print_table_row(2, "json version", PHP_JSON_VERSION);
	php_info_print_table_row(2, "json-c version", JSON_C_VERSION);
	php_info_print_table_end();
}
/* }}} */

static struct json_object *json_encode_r(zval *val);

static int json_determine_array_type(zval **val TSRMLS_DC) {
    int i;
    HashTable *myht;
  
    if (Z_TYPE_PP(val) == IS_ARRAY) {
        myht = HASH_OF(*val);
    } else {
        myht = Z_OBJPROP_PP(val);
        return 1;
    }

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

static struct json_object *json_encode_array(zval **val TSRMLS_DC) {
    int i, r;
    HashTable *myht;
    struct json_object *obj;
  
    if (Z_TYPE_PP(val) == IS_ARRAY) {
        myht = HASH_OF(*val);
        r = json_determine_array_type(val TSRMLS_CC);
    } else {
        myht = Z_OBJPROP_PP(val);
        r = 1;
    }

    if (r == 0 /* all keys numeric */) {
        obj = json_object_new_array();
    } else {
        obj = json_object_new_object();
    }

    i = myht ? zend_hash_num_elements(myht) : 0;
    if (i > 0) {
        char *key;
        zval **data;
        ulong index;
        uint key_len;
        HashPosition pos;
        struct json_object *member;
        char buffer[11];

        zend_hash_internal_pointer_reset_ex(myht, &pos);
        for (;; zend_hash_move_forward_ex(myht, &pos)) {
            i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
            if (i == HASH_KEY_NON_EXISTANT)
                break;

            if (zend_hash_get_current_data_ex(myht, (void **) &data, &pos) == SUCCESS) {
                member = json_encode_r(*data);
                if (r == 0) {
                    json_object_array_add(obj, member);
                } else if (r == 1) {
                    if (i == HASH_KEY_IS_STRING) {
                        if (key[0] == '\0') {
                            /* Skip protected and private members. */
                            if (member != NULL)
                                json_object_put(member);
                            continue;
                        }

                        json_object_object_add(obj, key, member);
                    } else {
                        snprintf(buffer, sizeof(buffer), "%ld", index);
                        buffer[10] = 0;
                        json_object_object_add(obj, buffer, member);
                    }
                }
            }
        }
    }

    return obj;
}

static struct json_object *json_encode_r(zval *val) {
    struct json_object *jo;

    switch (Z_TYPE_P(val)) {
        case IS_NULL:
            jo = NULL;
            break;
        case IS_BOOL:
            jo = json_object_new_boolean(Z_BVAL_P(val));
            break;
        case IS_LONG:
            jo = json_object_new_int(Z_LVAL_P(val));
            break;
        case IS_DOUBLE:
            jo = json_object_new_double(Z_DVAL_P(val));
            break;
        case IS_STRING:
            jo = json_object_new_string_len(Z_STRVAL_P(val), Z_STRLEN_P(val));
            break;
        case IS_ARRAY:
            jo = json_encode_array(&val TSRMLS_CC);
            break;
        case IS_OBJECT:
            jo = json_encode_array(&val TSRMLS_CC);
            break;
        default:
            zend_error(E_WARNING, "[json] (json_encode) type is unsupported\n");
            jo = NULL;
            break;
    }

    return jo;
}

PHP_FUNCTION(json_encode)
{
    zval *parameter;
    struct json_object *jo;
    char *s;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &parameter) == FAILURE) {
        return;
    }

    jo = json_encode_r(parameter);

    s = estrdup(json_object_to_json_string(jo));

    json_object_put(jo);

    RETURN_STRING(s, 0);
}

static zval *json_decode_r(struct json_object *jo, zend_bool assoc TSRMLS_DC) {
    zval *return_value;

    MAKE_STD_ZVAL(return_value);

    switch (json_object_get_type(jo)) {
      case json_type_boolean:
        ZVAL_BOOL(return_value, json_object_get_boolean(jo));
        break;
      case json_type_double:
        ZVAL_DOUBLE(return_value, json_object_get_double(jo));
        break;
      case json_type_int:
        ZVAL_LONG(return_value, json_object_get_int(jo));
        break;
      case json_type_object: {
        zval *mval;
		struct json_object_iter iter;

        if (assoc) {
          array_init(return_value);
        } else {
          object_init(return_value);
        }
		
        json_object_object_foreachC(jo, iter) {
          if (iter.val) {
            mval = json_decode_r(iter.val, assoc TSRMLS_CC);
          } else {
            MAKE_STD_ZVAL(mval);
            ZVAL_NULL(mval);
          }

          if (assoc) {
            add_assoc_zval(return_value, iter.key, mval);
          } else {
            add_property_zval(return_value, iter.key, mval);
#if PHP_MAJOR_VERSION >= 5
            ZVAL_DELREF(mval);
#endif
          }
        }
      }
        break;
      case json_type_array: {
        zval *mval;
        struct json_object *val;
        int i = 0, l;

        array_init(return_value);
        l = json_object_array_length(jo);
        for (i = 0; i < l; i++) {
          val = json_object_array_get_idx(jo, i);
          if (val) {
            mval = json_decode_r(val, assoc TSRMLS_CC);
          } else {
            MAKE_STD_ZVAL(mval);
            ZVAL_NULL(mval);
          }
          add_index_zval(return_value, i, mval);
        }
      }
      break;
      case json_type_string: {
        char *s =  json_object_get_string(jo);
        ZVAL_STRING(return_value, s, 1);
        break;
      }
        
      default:
        ZVAL_NULL(return_value);        
        break;
    }

    return return_value;
}

PHP_FUNCTION(json_decode)
{
    char *parameter;
    int parameter_len;
    zend_bool assoc = 0; /* return JS objects as PHP objects by default */
    struct json_object *jo;
    zval *z;
 
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &parameter, &parameter_len, &assoc) == FAILURE) {
        return;
    }

    jo = json_tokener_parse(parameter);
    if (!jo) {
      RETURN_NULL();
    }

    z = json_decode_r(jo, assoc TSRMLS_CC);
    if (!z) {
        RETURN_NULL();
    }
 
    json_object_put(jo);

    *return_value = *z;

    FREE_ZVAL(z);

    return;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
