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
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#include "normalizer_class.h"
#include "php_intl.h"
#include "normalizer_normalize.h"
#include "normalizer_arginfo.h"
#include "intl_error.h"

#include <unicode/unorm.h>

zend_class_entry *Normalizer_ce_ptr = NULL;

/*
 * 'Normalizer' class registration structures & functions
 */

/* {{{ Normalizer_class_functions
 * Every 'Normalizer' class method has an entry in this table
 */

static const zend_function_entry Normalizer_class_functions[] = {
	ZEND_FENTRY( normalize, ZEND_FN( normalizer_normalize ), arginfo_class_Normalizer_normalize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( isNormalized, ZEND_FN( normalizer_is_normalized ), arginfo_class_Normalizer_isNormalized, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
#if U_ICU_VERSION_MAJOR_NUM >= 56
	ZEND_FENTRY( getRawDecomposition, ZEND_FN( normalizer_get_raw_decomposition ), arginfo_class_Normalizer_getRawDecomposition, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
#endif
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
