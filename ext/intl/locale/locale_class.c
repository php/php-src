/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/

#include <unicode/uloc.h>
#include "php_intl.h"
#include "intl_error.h"
#include "locale_class.h"
#include "locale.h"
#include "locale_arginfo.h"

zend_class_entry *Locale_ce_ptr = NULL;

/*
 * 'Locale' class registration structures & functions
 */

/* {{{ locale_register_Locale_class
 * Initialize 'Locale' class
 */
void locale_register_Locale_class( void )
{
	zend_class_entry ce;

	/* Create and register 'Locale' class. */
	INIT_CLASS_ENTRY( ce, "Locale", class_Locale_methods );
	ce.create_object = NULL;
	Locale_ce_ptr = zend_register_internal_class( &ce );

	/* Declare 'Locale' class properties. */
	if( !Locale_ce_ptr )
	{
		zend_error( E_ERROR,
			"Locale: Failed to register Locale class.");
		return;
	}
}
/* }}} */
