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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#include <unicode/unum.h>

#include "msgformat_class.h"
#include "php_intl.h"
#include "msgformat_data.h"
#include "msgformat_format.h"
#include "msgformat_parse.h"
#include "msgformat.h"
#include "msgformat_attr.h"

zend_class_entry *MessageFormatter_ce_ptr = NULL;

/*
 * Auxiliary functions needed by objects of 'MessageFormatter' class
 */

/* {{{ MessageFormatter_objects_dtor */
static void MessageFormatter_object_dtor(void *object, zend_object_handle handle TSRMLS_DC )
{
	zend_objects_destroy_object( object, handle TSRMLS_CC );
}
/* }}} */

/* {{{ MessageFormatter_objects_free */
void MessageFormatter_object_free( zend_object *object TSRMLS_DC )
{
	MessageFormatter_object* mfo = (MessageFormatter_object*)object;

	zend_object_std_dtor( &mfo->zo TSRMLS_CC );

	msgformat_data_free( &mfo->mf_data TSRMLS_CC );

	efree( mfo );
}
/* }}} */

/* {{{ MessageFormatter_object_create */
zend_object_value MessageFormatter_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value    retval;
	MessageFormatter_object*     intern;

	intern = ecalloc( 1, sizeof(MessageFormatter_object) );
	msgformat_data_init( &intern->mf_data TSRMLS_CC );
	zend_object_std_init( &intern->zo, ce TSRMLS_CC );

	retval.handle = zend_objects_store_put(
		intern,
		MessageFormatter_object_dtor,
		(zend_objects_free_object_storage_t)MessageFormatter_object_free,
		NULL TSRMLS_CC );

	retval.handlers = zend_get_std_object_handlers();

	return retval;
}
/* }}} */

/*
 * 'MessageFormatter' class registration structures & functions
 */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_messageformatter___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_messageformatter_geterrormessage, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_messageformatter_formatmessage, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_messageformatter_format, 0, 0, 1)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_messageformatter_setpattern, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_messageformatter_parse, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ MessageFormatter_class_functions
 * Every 'MessageFormatter' class method has an entry in this table
 */
static function_entry MessageFormatter_class_functions[] = {
	PHP_ME( MessageFormatter, __construct, arginfo_messageformatter___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR )
	ZEND_FENTRY(  create, ZEND_FN( msgfmt_create ), arginfo_messageformatter___construct, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_NAMED_FE( format, ZEND_FN( msgfmt_format ), arginfo_messageformatter_format )
	ZEND_FENTRY(  formatMessage, ZEND_FN( msgfmt_format_message ), arginfo_messageformatter_formatmessage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_NAMED_FE( parse, ZEND_FN( msgfmt_parse ), arginfo_messageformatter_parse )
	ZEND_FENTRY(  parseMessage, ZEND_FN( msgfmt_parse_message ), arginfo_messageformatter_formatmessage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_NAMED_FE( setPattern, ZEND_FN( msgfmt_set_pattern ), arginfo_messageformatter_setpattern )
	PHP_NAMED_FE( getPattern, ZEND_FN( msgfmt_get_pattern ), arginfo_messageformatter_geterrormessage )
	PHP_NAMED_FE( getLocale, ZEND_FN( msgfmt_get_locale ), arginfo_messageformatter_geterrormessage )
	PHP_NAMED_FE( getErrorCode, ZEND_FN( msgfmt_get_error_code ), arginfo_messageformatter_geterrormessage )
	PHP_NAMED_FE( getErrorMessage, ZEND_FN( msgfmt_get_error_message ), arginfo_messageformatter_geterrormessage )
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ msgformat_register_class
 * Initialize 'MessageFormatter' class
 */
void msgformat_register_class( TSRMLS_D )
{
	zend_class_entry ce;

	/* Create and register 'MessageFormatter' class. */
	INIT_CLASS_ENTRY( ce, "MessageFormatter", MessageFormatter_class_functions );
	ce.create_object = MessageFormatter_object_create;
	MessageFormatter_ce_ptr = zend_register_internal_class( &ce TSRMLS_CC );

	/* Declare 'MessageFormatter' class properties. */
	if( !MessageFormatter_ce_ptr )
	{
		zend_error(E_ERROR, "Failed to register MessageFormatter class");
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
