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
static zend_object_handlers IntlDateFormatter_handlers;

/*
 * Auxiliary functions needed by objects of 'IntlDateFormatter' class
 */

/* {{{ IntlDateFormatter_objects_dtor */
static void IntlDateFormatter_object_dtor(void *object, zend_object_handle handle TSRMLS_DC )
{
	zend_objects_destroy_object( object, handle TSRMLS_CC );
}
/* }}} */

/* {{{ IntlDateFormatter_objects_free */
void IntlDateFormatter_object_free( zend_object *object TSRMLS_DC )
{
	IntlDateFormatter_object* dfo = (IntlDateFormatter_object*)object;

	zend_object_std_dtor( &dfo->zo TSRMLS_CC );

	dateformat_data_free( &dfo->datef_data TSRMLS_CC );
	
	if( dfo->timezone_id ){
		efree(dfo->timezone_id);
	}

	efree( dfo );
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
	intern->calendar  = 1;		/* Gregorian calendar */
	intern->timezone_id =  NULL;

	retval.handle = zend_objects_store_put(
		intern,
		IntlDateFormatter_object_dtor,
		(zend_objects_free_object_storage_t)IntlDateFormatter_object_free,
		NULL TSRMLS_CC );

	retval.handlers = &IntlDateFormatter_handlers;

	return retval;
}
/* }}} */

/* {{{ IntlDateFormatter_object_clone */
zend_object_value IntlDateFormatter_object_clone(zval *object TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object_handle handle = Z_OBJ_HANDLE_P(object);
	IntlDateFormatter_object *dfo, *new_dfo;

	DATE_FORMAT_METHOD_FETCH_OBJECT;
	new_obj_val = IntlDateFormatter_ce_ptr->create_object(IntlDateFormatter_ce_ptr TSRMLS_CC);
	new_dfo = (IntlDateFormatter_object *)zend_object_store_get_object_by_handle(new_obj_val.handle TSRMLS_CC);
	/* clone standard parts */	
	zend_objects_clone_members(&new_dfo->zo, new_obj_val, &dfo->zo, handle TSRMLS_CC);
	/* clone formatter object */
	DATE_FORMAT_OBJECT(new_dfo) = udat_clone(DATE_FORMAT_OBJECT(dfo),  &INTL_DATA_ERROR_CODE(new_dfo));
	if(U_FAILURE(INTL_DATA_ERROR_CODE(new_dfo))) {
		/* set up error in case error handler is interested */
		intl_error_set( NULL, INTL_DATA_ERROR_CODE(new_dfo), "Failed to clone IntlDateFormatter object", 0 TSRMLS_CC );
		IntlDateFormatter_object_dtor(new_dfo, new_obj_val.handle TSRMLS_CC); /* free new object */
		zend_error(E_ERROR, "Failed to clone IntlDateFormatter object");
	}
	return new_obj_val;
}
/* }}} */

/* 
 * 'IntlDateFormatter' class registration structures & functions
 */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(datefmt_parse_args, 0, 0, 1)
		ZEND_ARG_INFO(0, string)
		ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_format, 0, 0, 0)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_intldateformatter_getdatetype, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_settimezoneid, 0, 0, 1)
	ZEND_ARG_INFO(0, zone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_setpattern, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_setlenient, 0, 0, 1)
	ZEND_ARG_INFO(0, lenient)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter_setcalendar, 0, 0, 1)
	ZEND_ARG_INFO(0, which)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intldateformatter___construct, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, datetype)
	ZEND_ARG_INFO(0, timetype)
	ZEND_ARG_INFO(0, timezone)
	ZEND_ARG_INFO(0, calendar)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ IntlDateFormatter_class_functions
 * Every 'IntlDateFormatter' class method has an entry in this table
 */
static zend_function_entry IntlDateFormatter_class_functions[] = {
	PHP_ME( IntlDateFormatter, __construct, arginfo_intldateformatter___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR )
	ZEND_FENTRY(  create, ZEND_FN( datefmt_create ), arginfo_intldateformatter___construct, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_NAMED_FE( getDateType, ZEND_FN( datefmt_get_datetype ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getTimeType, ZEND_FN( datefmt_get_timetype ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getCalendar, ZEND_FN( datefmt_get_calendar ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( setCalendar, ZEND_FN( datefmt_set_calendar ), arginfo_intldateformatter_setcalendar )
	PHP_NAMED_FE( getTimeZoneId, ZEND_FN( datefmt_get_timezone_id ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( setTimeZoneId, ZEND_FN( datefmt_set_timezone_id ), arginfo_intldateformatter_settimezoneid )
	PHP_NAMED_FE( setPattern, ZEND_FN( datefmt_set_pattern ), arginfo_intldateformatter_setpattern )
	PHP_NAMED_FE( getPattern, ZEND_FN( datefmt_get_pattern ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getLocale, ZEND_FN( datefmt_get_locale ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( setLenient, ZEND_FN( datefmt_set_lenient ), arginfo_intldateformatter_setlenient )
	PHP_NAMED_FE( isLenient, ZEND_FN( datefmt_is_lenient ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( format, ZEND_FN( datefmt_format ), arginfo_intldateformatter_format )
	PHP_NAMED_FE( parse, ZEND_FN( datefmt_parse), datefmt_parse_args )
	PHP_NAMED_FE( localtime, ZEND_FN( datefmt_localtime ), datefmt_parse_args )
	PHP_NAMED_FE( getErrorCode, ZEND_FN( datefmt_get_error_code ), arginfo_intldateformatter_getdatetype )
	PHP_NAMED_FE( getErrorMessage, ZEND_FN( datefmt_get_error_message ), arginfo_intldateformatter_getdatetype )
	PHP_FE_END
};
/* }}} */

/* {{{ dateformat_register_class
 * Initialize 'IntlDateFormatter' class
 */
void dateformat_register_IntlDateFormatter_class( TSRMLS_D )
{
	zend_class_entry ce;

	/* Create and register 'IntlDateFormatter' class. */
	INIT_CLASS_ENTRY( ce, "IntlDateFormatter", IntlDateFormatter_class_functions );
	ce.create_object = IntlDateFormatter_object_create;
	IntlDateFormatter_ce_ptr = zend_register_internal_class( &ce TSRMLS_CC );

	memcpy(&IntlDateFormatter_handlers, zend_get_std_object_handlers(),
		sizeof IntlDateFormatter_handlers);
	IntlDateFormatter_handlers.clone_obj = IntlDateFormatter_object_clone;

	/* Declare 'IntlDateFormatter' class properties. */
	if( !IntlDateFormatter_ce_ptr )
	{
		zend_error(E_ERROR, "Failed to register IntlDateFormatter class");
		return;
	}
}
/* }}} */
