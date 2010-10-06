/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "transliterator_class.h"
#include "transliterator.h"
#include "intl_convert.h"

#include <unicode/ustring.h>

/* {{{ transliterator_register_constants
 * Register constants common for both (OO and procedural) APIs.
 */
void transliterator_register_constants( INIT_FUNC_ARGS )
{
	if( !Transliterator_ce_ptr )
	{
		zend_error( E_ERROR, "Transliterator class not defined" );
		return;
	}

	#define TRANSLITERATOR_EXPOSE_CONST( x ) REGISTER_LONG_CONSTANT( #x, x, CONST_CS )
	#define TRANSLITERATOR_EXPOSE_CLASS_CONST( x ) zend_declare_class_constant_long( Transliterator_ce_ptr, ZEND_STRS( #x ) - 1, TRANSLITERATOR_##x TSRMLS_CC );
	#define TRANSLITERATOR_EXPOSE_CUSTOM_CLASS_CONST( name, value ) zend_declare_class_constant_long( Transliterator_ce_ptr, ZEND_STRS( name ) - 1, value TSRMLS_CC );*/

	/* Normalization form constants */
	TRANSLITERATOR_EXPOSE_CLASS_CONST( FORWARD );
	TRANSLITERATOR_EXPOSE_CLASS_CONST( REVERSE );

	#undef NORMALIZER_EXPOSE_CUSTOM_CLASS_CONST
	#undef NORMALIZER_EXPOSE_CLASS_CONST
	#undef NORMALIZER_EXPOSE_CONST
}
/* }}} */

/* {{{ transliterator_parse_error_to_string
 * Transforms parse errors in strings.
 */
smart_str transliterator_parse_error_to_string( UParseError* pe )
{
	smart_str  ret = {0};
	char       *buf;
	int        u8len;
	UErrorCode status;
	int        any = 0;

	assert( pe != NULL );

	smart_str_appends( &ret, "parse error " );
	if( pe->line > 0 )
	{
		smart_str_appends( &ret, "on line " );
		smart_str_append_long( &ret, (long ) pe->line );
		any = 1;
	}
	if( pe->offset >= 0 ) {
		if( any )
			smart_str_appends( &ret, ", " );
		else
			smart_str_appends( &ret, "at " );

		smart_str_appends( &ret, "offset " );
		smart_str_append_long( &ret, (long ) pe->offset ); 
		any = 1;
	}

	if (pe->preContext[0] != 0 ) {
		if( any )
			smart_str_appends( &ret, ", " );

		smart_str_appends( &ret, "after \"" );
		intl_convert_utf16_to_utf8( &buf, &u8len, pe->preContext, -1, &status );
		if( U_FAILURE( status ) )
		{
			smart_str_appends( &ret, "(could not convert parser error pre-context to UTF-8)" );
		}
		else {
			smart_str_appendl( &ret, buf, u8len );
			efree( buf );
		}
		smart_str_appends( &ret, "\"" );
		any = 1;
	}

	if( pe->postContext[0] != 0 )
	{
		if( any )
			smart_str_appends( &ret, ", " );

		smart_str_appends( &ret, "before or at \"" );
		intl_convert_utf16_to_utf8( &buf, &u8len, pe->postContext, -1, &status );
		if( U_FAILURE( status ) )
		{
			smart_str_appends( &ret, "(could not convert parser error post-context to UTF-8)" );
		}
		else
		{
			smart_str_appendl( &ret, buf, u8len );
			efree( buf );
		}
		smart_str_appends( &ret, "\"" );
		any = 1;
	}

	if( !any )
	{
		smart_str_free( &ret );
		smart_str_appends( &ret, "no parse error" );
	}
	
	smart_str_0( &ret );
	return ret;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
