/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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


U_CFUNC void locale_register_Locale_class( void );

extern zend_class_entry *Locale_ce_ptr;

/* Auxiliary macros */

#define LOCALE_METHOD_INIT_VARS       \
    zval*             	object  = NULL;   \
    intl_error_reset( NULL ); \

#endif // #ifndef LOCALE_CLASS_H
