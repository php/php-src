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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "collator_class.h"
#include "collator.h"

#include <unicode/utypes.h>
#include <unicode/ucol.h>
#include <unicode/ustring.h>

/* {{{ collator_register_constants
 * Register constants common for the both (OO and procedural)
 * APIs.
 */
void collator_register_constants( INIT_FUNC_ARGS )
{
	if( !Collator_ce_ptr )
	{
		zend_error( E_ERROR, "Collator class not defined" );
		return;
	}

	#define COLLATOR_EXPOSE_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_PERSISTENT | CONST_CS)
	#define COLLATOR_EXPOSE_CLASS_CONST(x) zend_declare_class_constant_long( Collator_ce_ptr, ZEND_STRS( #x ) - 1, UCOL_##x );
	#define COLLATOR_EXPOSE_CUSTOM_CLASS_CONST(name, value) zend_declare_class_constant_long( Collator_ce_ptr, ZEND_STRS( name ) - 1, value );

	/* UColAttributeValue constants */
	COLLATOR_EXPOSE_CUSTOM_CLASS_CONST( "DEFAULT_VALUE", UCOL_DEFAULT );

	COLLATOR_EXPOSE_CLASS_CONST( PRIMARY );
	COLLATOR_EXPOSE_CLASS_CONST( SECONDARY );
	COLLATOR_EXPOSE_CLASS_CONST( TERTIARY );
	COLLATOR_EXPOSE_CLASS_CONST( DEFAULT_STRENGTH );
	COLLATOR_EXPOSE_CLASS_CONST( QUATERNARY );
	COLLATOR_EXPOSE_CLASS_CONST( IDENTICAL );

	COLLATOR_EXPOSE_CLASS_CONST( OFF );
	COLLATOR_EXPOSE_CLASS_CONST( ON );

	COLLATOR_EXPOSE_CLASS_CONST( SHIFTED );
	COLLATOR_EXPOSE_CLASS_CONST( NON_IGNORABLE );

	COLLATOR_EXPOSE_CLASS_CONST( LOWER_FIRST );
	COLLATOR_EXPOSE_CLASS_CONST( UPPER_FIRST );

	/* UColAttribute constants */
	COLLATOR_EXPOSE_CLASS_CONST( FRENCH_COLLATION );
	COLLATOR_EXPOSE_CLASS_CONST( ALTERNATE_HANDLING );
	COLLATOR_EXPOSE_CLASS_CONST( CASE_FIRST );
	COLLATOR_EXPOSE_CLASS_CONST( CASE_LEVEL );
	COLLATOR_EXPOSE_CLASS_CONST( NORMALIZATION_MODE );
	COLLATOR_EXPOSE_CLASS_CONST( STRENGTH );
	COLLATOR_EXPOSE_CLASS_CONST( HIRAGANA_QUATERNARY_MODE );
	COLLATOR_EXPOSE_CLASS_CONST( NUMERIC_COLLATION );

	/* ULocDataLocaleType constants */
	COLLATOR_EXPOSE_CONST( ULOC_ACTUAL_LOCALE );
	COLLATOR_EXPOSE_CONST( ULOC_VALID_LOCALE );

	/* sort flags */
	COLLATOR_EXPOSE_CUSTOM_CLASS_CONST( "SORT_REGULAR", COLLATOR_SORT_REGULAR );
	COLLATOR_EXPOSE_CUSTOM_CLASS_CONST( "SORT_STRING",  COLLATOR_SORT_STRING  );
	COLLATOR_EXPOSE_CUSTOM_CLASS_CONST( "SORT_NUMERIC", COLLATOR_SORT_NUMERIC );

	#undef COLLATOR_EXPOSE_CUSTOM_CLASS_CONST
	#undef COLLATOR_EXPOSE_CLASS_CONST
	#undef COLLATOR_EXPOSE_CONST
}
/* }}} */
