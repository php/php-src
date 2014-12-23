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

	#define TRANSLITERATOR_EXPOSE_CONST( x ) REGISTER_LONG_CONSTANT( #x, x, CONST_PERSISTENT | CONST_CS )
	#define TRANSLITERATOR_EXPOSE_CLASS_CONST( x ) zend_declare_class_constant_long( Transliterator_ce_ptr, ZEND_STRS( #x ) - 1, TRANSLITERATOR_##x );
	#define TRANSLITERATOR_EXPOSE_CUSTOM_CLASS_CONST( name, value ) zend_declare_class_constant_long( Transliterator_ce_ptr, ZEND_STRS( name ) - 1, value );*/

	/* Normalization form constants */
	TRANSLITERATOR_EXPOSE_CLASS_CONST( FORWARD );
	TRANSLITERATOR_EXPOSE_CLASS_CONST( REVERSE );

	#undef NORMALIZER_EXPOSE_CUSTOM_CLASS_CONST
	#undef NORMALIZER_EXPOSE_CLASS_CONST
	#undef NORMALIZER_EXPOSE_CONST
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
