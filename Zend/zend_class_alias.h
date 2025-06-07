/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Daniel Scherzer <daniel.e.scherzer@gmail.com>               |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CLASS_ALIAS_H
#define ZEND_CLASS_ALIAS_H

#include "zend_types.h"

struct _zend_class_alias {
    zend_refcounted_h gc;
    zend_class_entry *ce;
    zend_string *name;
    HashTable *attributes;
    uint32_t alias_flags;
};

typedef struct _zend_class_alias zend_class_alias;

#define Z_CE_FROM_ZVAL_P(_ce, _zv) do {                 \
        if (EXPECTED(Z_TYPE_P(_zv) == IS_PTR)) {        \
            _ce = Z_PTR_P(_zv);                         \
        } else {                                        \
            ZEND_ASSERT(Z_TYPE_P(_zv) == IS_ALIAS_PTR); \
            _ce = Z_CLASS_ALIAS_P(_zv)->ce;             \
        }                                               \
    } while (0)                                         \


#define Z_CE_FROM_ZVAL(_ce, _zv) do {                   \
        if (EXPECTED(Z_TYPE(_zv) == IS_PTR)) {          \
            _ce = Z_PTR(_zv);                           \
        } else {                                        \
            ZEND_ASSERT(Z_TYPE(_zv) == IS_ALIAS_PTR);   \
            _ce = Z_CLASS_ALIAS(_zv)->ce;               \
        }                                               \
    } while (0)                                         \


zend_class_alias * zend_class_alias_init(zend_class_entry *ce);
ZEND_API ZEND_COLD void ZEND_FASTCALL zend_deprecated_class_alias(const zend_class_alias *alias);

#endif
