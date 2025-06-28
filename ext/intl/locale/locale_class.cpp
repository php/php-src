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

#include <unicode/uloc.h>
extern "C" {
#include "php_intl.h"
#include "intl_error.h"
#include "locale_class.h"
#include "locale.h"
#include "locale_arginfo.h"
}

zend_class_entry *Locale_ce_ptr = NULL;

/*
 * 'Locale' class registration structures & functions
 */

/* {{{ locale_register_Locale_class
 * Initialize 'Locale' class
 */
U_CFUNC void locale_register_Locale_class( void )
{
	/* Create and register 'Locale' class. */
	Locale_ce_ptr = register_class_Locale();
	Locale_ce_ptr->create_object = NULL;
}
/* }}} */
