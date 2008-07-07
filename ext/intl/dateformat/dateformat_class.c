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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#include <unicode/unum.h>

#include "dateformat_class.h"
#include "php_intl.h"
#include "dateformat_data.h"
#include "dateformat_format.h"
#include "dateformat_parse.h"
#include "dateformat.h"
#include "dateformat_attr.h"

zend_class_entry *IntlDateFormatter_ce_ptr = NULL;

/////////////////////////////////////////////////////////////////////////////
// Auxiliary functions needed by objects of 'IntlDateFormatter' class
/////////////////////////////////////////////////////////////////////////////

/* {{{ IntlDateFormatter_objects_dtor */
static void IntlDateFormatter_object_dtor(void *object, zend_object_handle handle TSRMLS_DC )
{
	zend_objects_destroy_object( object, handle TSRMLS_CC );
}
/* }}} */

/* {{{ IntlDateFormatter_objects_free */
void IntlDateFormatter_object_free( zend_object *object TSRMLS_DC )
{
	IntlDateFormatter_object* mfo = (IntlDateFormatter_object*)object;

	zend_object_std_dtor( &mfo->zo TSRMLS_CC );

	dateformat_data_free( &mfo->datef_data TSRMLS_CC );
	
	if( mfo->timezone_id ){
		efree(mfo->timezone_id);
	}

	efree( mfo );
}
/* }}} */

/* {{{ IntlDateFormatter_object_create */
zend_object_value IntlDateFormatter_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value    retval;
	IntlDateFormatter_object*     intern;

	intern = ecalloc( 1, sizeof(IntlDateFormatter_object) );
	dateformat_data_init( &intern->datef_data TSRMLS_CC );
	zend_object_std_init( &intern->zo, ce TSRMLS_CC );
	intern->date_type = 0;
	intern->time_type = 0;
	intern->calendar  = 1;		//Gregorian calendar
	intern->timezone_id =  NULL;

	retval.handle = zend_objects_store_put(
		intern,
		IntlDateFormatter_object_dtor,
		(zend_objects_free_object_storage_t)IntlDateFormatter_object_free,
		NULL TSRMLS_CC );

	retval.handlers = zend_get_std_object_handlers();

	return retval;
}
/* }}} */

/////////////////////////////////////////////////////////////////////////////
// 'IntlDateFormatter' class registration structures & functions
/////////////////////////////////////////////////////////////////////////////

/* {{{ IntlDateFormatter_class_functions
 * Every 'IntlDateFormatter' class method has an entry in this table
 */

static function_entry IntlDateFormatter_class_functions[] = {
	PHP_ME( IntlDateFormatter, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR )
	ZEND_FENTRY(  create, ZEND_FN( datefmt_create ), NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_NAMED_FE( getDateType, ZEND_FN( datefmt_get_datetype ), NULL )
	PHP_NAMED_FE( getTimeType, ZEND_FN( datefmt_get_timetype ), NULL )
	PHP_NAMED_FE( getCalendar, ZEND_FN( datefmt_get_calendar ), NULL )
	PHP_NAMED_FE( setCalendar, ZEND_FN( datefmt_set_calendar ), NULL )
	PHP_NAMED_FE( getTimeZoneId, ZEND_FN( datefmt_get_timezone_id ), NULL )
	PHP_NAMED_FE( setTimeZoneId, ZEND_FN( datefmt_set_timezone_id ), NULL )
	PHP_NAMED_FE( setPattern, ZEND_FN( datefmt_set_pattern ), NULL )
	PHP_NAMED_FE( getPattern, ZEND_FN( datefmt_get_pattern ), NULL )
	PHP_NAMED_FE( getLocale, ZEND_FN( datefmt_get_locale ), NULL )
	PHP_NAMED_FE( setLenient, ZEND_FN( datefmt_set_lenient ), NULL )
	PHP_NAMED_FE( isLenient, ZEND_FN( datefmt_is_lenient ), NULL )
	PHP_NAMED_FE( format, ZEND_FN( datefmt_format ), NULL )
	PHP_NAMED_FE( parse, ZEND_FN( datefmt_parse), NULL )
	PHP_NAMED_FE( localtime, ZEND_FN( datefmt_localtime ), NULL )
	PHP_NAMED_FE( getErrorCode, ZEND_FN( datefmt_get_error_code ), NULL )
	PHP_NAMED_FE( getErrorMessage, ZEND_FN( datefmt_get_error_message ), NULL )
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ dateformat_register_class
 * Initialize 'IntlDateFormatter' class
 */
void dateformat_register_IntlDateFormatter_class( TSRMLS_D )
{
	zend_class_entry ce;

	// Create and register 'IntlDateFormatter' class.
	INIT_CLASS_ENTRY( ce, "IntlDateFormatter", IntlDateFormatter_class_functions );
	ce.create_object = IntlDateFormatter_object_create;
	IntlDateFormatter_ce_ptr = zend_register_internal_class( &ce TSRMLS_CC );

	// Declare 'IntlDateFormatter' class properties.
	if( !IntlDateFormatter_ce_ptr )
	{
		zend_error(E_ERROR, "Failed to register IntlDateFormatter class");
		return;
	}
}
/* }}} */
