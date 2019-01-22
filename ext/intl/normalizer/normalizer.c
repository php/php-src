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
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "normalizer_class.h"
#include "normalizer.h"

#include <unicode/utypes.h>
#include <unicode/unorm.h>
#include <unicode/ustring.h>

/* {{{ normalizer_register_constants
 * Register constants common for the both (OO and procedural)
 * APIs.
 */
void normalizer_register_constants( INIT_FUNC_ARGS )
{
	if( !Normalizer_ce_ptr )
	{
		zend_error( E_ERROR, "Normalizer class not defined" );
		return;
	}

	#define NORMALIZER_EXPOSE_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_PERSISTENT | CONST_CS)
	#define NORMALIZER_EXPOSE_CLASS_CONST(x) zend_declare_class_constant_long( Normalizer_ce_ptr, ZEND_STRS( #x ) - 1, NORMALIZER_##x );
	#define NORMALIZER_EXPOSE_CUSTOM_CLASS_CONST(name, value) zend_declare_class_constant_long( Normalizer_ce_ptr, ZEND_STRS( name ) - 1, value );

	/* Normalization form constants */
	NORMALIZER_EXPOSE_CLASS_CONST( NONE );
	NORMALIZER_EXPOSE_CLASS_CONST( FORM_D );
	NORMALIZER_EXPOSE_CLASS_CONST( NFD );
	NORMALIZER_EXPOSE_CLASS_CONST( FORM_KD );
	NORMALIZER_EXPOSE_CLASS_CONST( NFKD );
	NORMALIZER_EXPOSE_CLASS_CONST( FORM_C );
	NORMALIZER_EXPOSE_CLASS_CONST( NFC );
	NORMALIZER_EXPOSE_CLASS_CONST( FORM_KC );
	NORMALIZER_EXPOSE_CLASS_CONST( NFKC );
#if U_ICU_VERSION_MAJOR_NUM >= 56
	NORMALIZER_EXPOSE_CLASS_CONST( FORM_KC_CF );
	NORMALIZER_EXPOSE_CLASS_CONST( NFKC_CF );
#endif

	#undef NORMALIZER_EXPOSE_CUSTOM_CLASS_CONST
	#undef NORMALIZER_EXPOSE_CLASS_CONST
	#undef NORMALIZER_EXPOSE_CONST
}
/* }}} */
