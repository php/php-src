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

#ifndef LISTFORMATTER_CLASS_H
#define LISTFORMATTER_CLASS_H

#include <php.h>

#include "intl_common.h"
#include "intl_error.h"
#include "intl_data.h"

#include <unicode/ulistformatter.h>

typedef struct {
    // error handling
    intl_error      error;

    // formatter handling
    UListFormatter*  ulistfmt;
} listformatter_data;

typedef struct {
    listformatter_data  lf_data;
    zend_object     zo;
} ListFormatter_object;

static inline ListFormatter_object *php_intl_listformatter_fetch_object(zend_object *obj) {
    return (ListFormatter_object *)((char*)(obj) - XtOffsetOf(ListFormatter_object, zo));
}
#define Z_INTL_LISTFORMATTER_P(zv) php_intl_listformatter_fetch_object(Z_OBJ_P(zv))

#define LISTFORMATTER_ERROR(lfo) (lfo)->lf_data.error
#define LISTFORMATTER_ERROR_P(lfo) &(LISTFORMATTER_ERROR(lfo))

#define LISTFORMATTER_OBJECT(lfo) (lfo)->lf_data.ulistfmt

void listformatter_register_class( void );
extern zend_class_entry *ListFormatter_ce_ptr;

#endif // LISTFORMATTER_CLASS_H 
