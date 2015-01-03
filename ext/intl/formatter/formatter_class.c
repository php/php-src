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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#include <unicode/unum.h>

#include "formatter_class.h"
#include "php_intl.h"
#include "formatter_data.h"
#include "formatter_format.h"
#include "formatter_parse.h"
#include "formatter_main.h"
#include "formatter_attr.h"

#include <zend_exceptions.h>

zend_class_entry *NumberFormatter_ce_ptr = NULL;
static zend_object_handlers NumberFormatter_handlers;

/*
 * Auxiliary functions needed by objects of 'NumberFormatter' class
 */

/* {{{ NumberFormatter_objects_dtor */
static void NumberFormatter_object_dtor(zend_object *object)
{
	zend_objects_destroy_object( object );
}
/* }}} */

/* {{{ NumberFormatter_objects_free */
void NumberFormatter_object_free( zend_object *object )
{
	NumberFormatter_object* nfo = php_intl_number_format_fetch_object(object);

	zend_object_std_dtor( &nfo->zo );

	formatter_data_free( &nfo->nf_data );
}
/* }}} */

/* {{{ NumberFormatter_object_create */
zend_object *NumberFormatter_object_create(zend_class_entry *ce)
{
	NumberFormatter_object*     intern;

	intern = ecalloc( 1, sizeof(NumberFormatter_object) + sizeof(zval) * (ce->default_properties_count - 1) );
	formatter_data_init( &intern->nf_data );
	zend_object_std_init( &intern->zo, ce );
	object_properties_init(&intern->zo, ce);

	intern->zo.handlers = &NumberFormatter_handlers;

	return &intern->zo;
}
/* }}} */

/* {{{ NumberFormatter_object_clone */
zend_object *NumberFormatter_object_clone(zval *object)
{
	NumberFormatter_object *nfo, *new_nfo;
	zend_object *new_obj;

	FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK;
	new_obj = NumberFormatter_ce_ptr->create_object(Z_OBJCE_P(object));
	new_nfo = php_intl_number_format_fetch_object(new_obj);
	/* clone standard parts */
	zend_objects_clone_members(&new_nfo->zo, &nfo->zo);
	/* clone formatter object. It may fail, the destruction code must handle this case */
	if (FORMATTER_OBJECT(nfo) != NULL) {
		FORMATTER_OBJECT(new_nfo) = unum_clone(FORMATTER_OBJECT(nfo),
				&INTL_DATA_ERROR_CODE(nfo));
		if (U_FAILURE(INTL_DATA_ERROR_CODE(nfo))) {
			/* set up error in case error handler is interested */
			intl_errors_set(INTL_DATA_ERROR_P(nfo), INTL_DATA_ERROR_CODE(nfo),
					"Failed to clone NumberFormatter object", 0);
			zend_throw_exception(NULL, "Failed to clone NumberFormatter object", 0);
		}
	} else {
		zend_throw_exception(NULL, "Cannot clone unconstructed NumberFormatter", 0);
	}
	return new_obj;
}
/* }}} */

/*
 * 'NumberFormatter' class registration structures & functions
 */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(number_parse_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(number_parse_currency_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, currency)
	ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numberformatter_getattribute, 0, 0, 1)
	ZEND_ARG_INFO(0, attr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numberformatter_setattribute, 0, 0, 2)
	ZEND_ARG_INFO(0, attr)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numberformatter_setsymbol, 0, 0, 2)
	ZEND_ARG_INFO(0, attr)
	ZEND_ARG_INFO(0, symbol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_numberformatter_getpattern, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numberformatter_setpattern, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numberformatter_getlocale, 0, 0, 0)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numberformatter___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, style)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numberformatter_formatcurrency, 0, 0, 2)
	ZEND_ARG_INFO(0, num)
	ZEND_ARG_INFO(0, currency)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numberformatter_format, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ NumberFormatter_class_functions
 * Every 'NumberFormatter' class method has an entry in this table
 */
static zend_function_entry NumberFormatter_class_functions[] = {
	PHP_ME( NumberFormatter, __construct, arginfo_numberformatter___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR )
	ZEND_FENTRY( create, ZEND_FN( numfmt_create ), arginfo_numberformatter___construct, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_NAMED_FE( format, ZEND_FN( numfmt_format ), arginfo_numberformatter_format )
	PHP_NAMED_FE( parse, ZEND_FN( numfmt_parse ), number_parse_arginfo )
	PHP_NAMED_FE( formatCurrency, ZEND_FN( numfmt_format_currency ), arginfo_numberformatter_formatcurrency )
	PHP_NAMED_FE( parseCurrency, ZEND_FN( numfmt_parse_currency ), number_parse_currency_arginfo )
	PHP_NAMED_FE( setAttribute, ZEND_FN( numfmt_set_attribute ), arginfo_numberformatter_setattribute )
	PHP_NAMED_FE( getAttribute, ZEND_FN( numfmt_get_attribute ), arginfo_numberformatter_getattribute )
	PHP_NAMED_FE( setTextAttribute, ZEND_FN( numfmt_set_text_attribute ), arginfo_numberformatter_setattribute )
	PHP_NAMED_FE( getTextAttribute, ZEND_FN( numfmt_get_text_attribute ), arginfo_numberformatter_getattribute )
	PHP_NAMED_FE( setSymbol, ZEND_FN( numfmt_set_symbol ), arginfo_numberformatter_setsymbol )
	PHP_NAMED_FE( getSymbol, ZEND_FN( numfmt_get_symbol ), arginfo_numberformatter_getattribute )
	PHP_NAMED_FE( setPattern, ZEND_FN( numfmt_set_pattern ), arginfo_numberformatter_setpattern )
	PHP_NAMED_FE( getPattern, ZEND_FN( numfmt_get_pattern ), arginfo_numberformatter_getpattern )
	PHP_NAMED_FE( getLocale, ZEND_FN( numfmt_get_locale ), arginfo_numberformatter_getlocale )
	PHP_NAMED_FE( getErrorCode, ZEND_FN( numfmt_get_error_code ), arginfo_numberformatter_getpattern )
	PHP_NAMED_FE( getErrorMessage, ZEND_FN( numfmt_get_error_message ), arginfo_numberformatter_getpattern )
	PHP_FE_END
};
/* }}} */

/* {{{ formatter_register_class
 * Initialize 'NumberFormatter' class
 */
void formatter_register_class( void )
{
	zend_class_entry ce;

	/* Create and register 'NumberFormatter' class. */
	INIT_CLASS_ENTRY( ce, "NumberFormatter", NumberFormatter_class_functions );
	ce.create_object = NumberFormatter_object_create;
	NumberFormatter_ce_ptr = zend_register_internal_class( &ce );

	memcpy(&NumberFormatter_handlers, zend_get_std_object_handlers(),
		sizeof(NumberFormatter_handlers));
	NumberFormatter_handlers.offset = XtOffsetOf(NumberFormatter_object, zo);
	NumberFormatter_handlers.clone_obj = NumberFormatter_object_clone;
	NumberFormatter_handlers.dtor_obj = NumberFormatter_object_dtor;
	NumberFormatter_handlers.free_obj = NumberFormatter_object_free;

	/* Declare 'NumberFormatter' class properties. */
	if( !NumberFormatter_ce_ptr )
	{
		zend_error(E_ERROR, "Failed to register NumberFormatter class");
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
