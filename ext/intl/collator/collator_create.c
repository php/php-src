/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php_intl.h"
#include "collator_class.h"
#include "intl_data.h"

/* {{{ */
static int collator_ctor(INTERNAL_FUNCTION_PARAMETERS, zend_error_handling *error_handling, bool *error_handling_replaced)
{
	const char*      locale;
	size_t           locale_len = 0;
	zval*            object;
	Collator_object* co;

	intl_error_reset( NULL );
	object = return_value;
	/* Parse parameters. */
	if( zend_parse_parameters( ZEND_NUM_ARGS(), "s",
		&locale, &locale_len ) == FAILURE )
	{
		return FAILURE;
	}

	if (error_handling != NULL) {
		zend_replace_error_handling(EH_THROW, IntlException_ce_ptr, error_handling);
		*error_handling_replaced = 1;
	}

	INTL_CHECK_LOCALE_LEN_OR_FAILURE(locale_len);
	COLLATOR_METHOD_FETCH_OBJECT;

	if(locale_len == 0) {
		locale = intl_locale_get_default();
	}

	/* Open ICU collator. */
	co->ucoll = ucol_open( locale, COLLATOR_ERROR_CODE_P( co ) );
	INTL_CTOR_CHECK_STATUS(co, "collator_create: unable to open ICU collator");
	return SUCCESS;
}
/* }}} */

/* {{{ Create collator. */
PHP_FUNCTION( collator_create )
{
	object_init_ex( return_value, Collator_ce_ptr );
	if (collator_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, NULL, NULL) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ Collator object constructor. */
PHP_METHOD( Collator, __construct )
{
	zend_error_handling error_handling;
	bool error_handling_replaced = 0;

	return_value = ZEND_THIS;
	if (collator_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, &error_handling, &error_handling_replaced) == FAILURE) {
		if (!EG(exception)) {
			zend_throw_exception(IntlException_ce_ptr, "Constructor failed", 0);
		}
	}
	if (error_handling_replaced) {
		zend_restore_error_handling(&error_handling);
	}
}
/* }}} */
