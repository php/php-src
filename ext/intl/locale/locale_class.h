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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/

#ifndef LOCALE_CLASS_H
#define LOCALE_CLASS_H

#include <php.h>

#include "intl_common.h"
#include "intl_error.h"

#include <unicode/uloc.h>

typedef struct {
	zend_object     zo;

	// ICU locale
	char*      		uloc1;

} Locale_object;


void locale_register_Locale_class( void );

extern zend_class_entry *Locale_ce_ptr;

/* Auxiliary macros */

#define LOCALE_METHOD_INIT_VARS       \
    zval*             	object  = NULL;   \
    intl_error_reset( NULL ); \

#endif // #ifndef LOCALE_CLASS_H
