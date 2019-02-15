/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Kirti Velankar <kirtig@yahoo-inc.com>                        |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "locale_class.h"
#include "locale.h"

#include <unicode/utypes.h>
#include <unicode/uloc.h>
#include <unicode/ustring.h>

/* {{{ locale_register_constants
 * Register constants common for the both (OO and procedural)
 * APIs.
 */
void locale_register_constants( INIT_FUNC_ARGS )
{
	if( !Locale_ce_ptr )
	{
		zend_error( E_ERROR, "Locale class not defined" );
		return;
	}

	#define LOCALE_EXPOSE_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_PERSISTENT | CONST_CS)
	#define LOCALE_EXPOSE_CLASS_CONST(x) zend_declare_class_constant_long( Locale_ce_ptr, ZEND_STRS( #x ) - 1, ULOC_##x );
	#define LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR(name, value) zend_declare_class_constant_string( Locale_ce_ptr, ZEND_STRS( name ) - 1, value );

	LOCALE_EXPOSE_CLASS_CONST( ACTUAL_LOCALE );
	LOCALE_EXPOSE_CLASS_CONST( VALID_LOCALE );

	zend_declare_class_constant_null(Locale_ce_ptr, ZEND_STRS("DEFAULT_LOCALE") - 1);

	LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR( "LANG_TAG", LOC_LANG_TAG);
	LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR( "EXTLANG_TAG", LOC_EXTLANG_TAG);
	LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR( "SCRIPT_TAG", LOC_SCRIPT_TAG);
	LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR( "REGION_TAG", LOC_REGION_TAG);
	LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR( "VARIANT_TAG",LOC_VARIANT_TAG);
	LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR( "GRANDFATHERED_LANG_TAG",LOC_GRANDFATHERED_LANG_TAG);
	LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR( "PRIVATE_TAG",LOC_PRIVATE_TAG);

	#undef LOCALE_EXPOSE_CUSTOM_CLASS_CONST_STR
	#undef LOCALE_EXPOSE_CLASS_CONST
	#undef LOCALE_EXPOSE_CONST
}
/* }}} */
