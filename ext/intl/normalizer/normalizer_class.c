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

#include "normalizer_class.h"
#include "php_intl.h"
#include "normalizer_normalize.h"
#include "intl_error.h"

#include <unicode/unorm.h>

zend_class_entry *Normalizer_ce_ptr = NULL;

/*
 * 'Normalizer' class registration structures & functions
 */

/* {{{ Normalizer methods arguments info */

ZEND_BEGIN_ARG_INFO_EX( normalizer_3_args, 0, 0, 3 )
	ZEND_ARG_INFO( 0, arg1 )
	ZEND_ARG_INFO( 0, arg2 )
	ZEND_ARG_INFO( 0, arg3 )
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ Normalizer_class_functions
 * Every 'Normalizer' class method has an entry in this table
 */

zend_function_entry Normalizer_class_functions[] = {
	ZEND_FENTRY( normalize, ZEND_FN( normalizer_normalize ), normalizer_3_args, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( isNormalized, ZEND_FN( normalizer_is_normalized ), normalizer_3_args, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_FE_END
};
/* }}} */

/* {{{ normalizer_register_Normalizer_class
 * Initialize 'Normalizer' class
 */
void normalizer_register_Normalizer_class( void )
{
	zend_class_entry ce;

	/* Create and register 'Normalizer' class. */
	INIT_CLASS_ENTRY( ce, "Normalizer", Normalizer_class_functions );
	ce.create_object = NULL;
	Normalizer_ce_ptr = zend_register_internal_class( &ce );

	/* Declare 'Normalizer' class properties. */
	if( !Normalizer_ce_ptr )
	{
		zend_error( E_ERROR,
			"Normalizer: attempt to create properties "
			"on a non-registered class." );
		return;
	}
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
