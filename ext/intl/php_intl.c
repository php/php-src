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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Kirti Velankar <kirtig@yahoo-inc.com>   			  |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php_intl.h"
#include "php_intl_arginfo.h"
#include "intl_error.h"
#include "collator/collator_class.h"
#include "collator/collator.h"
#include "collator/collator_sort.h"
#include "collator/collator_convert.h"

#include "converter/converter.h"

#include "formatter/formatter.h"
#include "formatter/formatter_class.h"
#include "formatter/formatter_format.h"

#include "grapheme/grapheme.h"

#include "msgformat/msgformat_class.h"

#include "normalizer/normalizer.h"
#include "normalizer/normalizer_class.h"

#include "locale/locale.h"
#include "locale/locale_class.h"

#include "dateformat/dateformat.h"
#include "dateformat/dateformat_class.h"
#include "dateformat/dateformat_data.h"

#include "resourcebundle/resourcebundle_class.h"

#include "transliterator/transliterator.h"
#include "transliterator/transliterator_class.h"

#include "timezone/timezone_class.h"

#include "calendar/calendar_class.h"

#include "breakiterator/breakiterator_class.h"
#include "breakiterator/breakiterator_iterators.h"

#include "idn/idn.h"
#include "uchar/uchar.h"

# include "spoofchecker/spoofchecker_class.h"
# include "spoofchecker/spoofchecker.h"

#include "common/common_error.h"
#include "common/common_enum.h"

#include <unicode/uloc.h>
#include <unicode/uclean.h>
#include <ext/standard/info.h>

#include "php_ini.h"

/*
 * locale_get_default has a conflict since ICU also has
 * a function with the same  name
 * in fact ICU appends the version no. to it also
 * Hence the following undef for ICU version
 * Same true for the locale_set_default function
*/
#undef locale_get_default
#undef locale_set_default

ZEND_DECLARE_MODULE_GLOBALS( intl )

const char *intl_locale_get_default( void )
{
	if( INTL_G(default_locale) == NULL ) {
		return uloc_getDefault();
	}
	return INTL_G(default_locale);
}

/* {{{ INI Settings */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY(LOCALE_INI_NAME, NULL, PHP_INI_ALL, OnUpdateStringUnempty, default_locale, zend_intl_globals, intl_globals)
    STD_PHP_INI_ENTRY("intl.error_level", "0", PHP_INI_ALL, OnUpdateLong, error_level, zend_intl_globals, intl_globals)
	STD_PHP_INI_BOOLEAN("intl.use_exceptions", "0", PHP_INI_ALL, OnUpdateBool, use_exceptions, zend_intl_globals, intl_globals)
PHP_INI_END()
/* }}} */

static PHP_GINIT_FUNCTION(intl);

/* {{{ intl_module_entry */
zend_module_entry intl_module_entry = {
	STANDARD_MODULE_HEADER,
	"intl",
	ext_functions,
	PHP_MINIT( intl ),
	PHP_MSHUTDOWN( intl ),
	PHP_RINIT( intl ),
	PHP_RSHUTDOWN( intl ),
	PHP_MINFO( intl ),
	PHP_INTL_VERSION,
	PHP_MODULE_GLOBALS(intl),   /* globals descriptor */
	PHP_GINIT(intl),            /* globals ctor */
	NULL,                       /* globals dtor */
	NULL,                       /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_INTL
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE( intl )
#endif

/* {{{ intl_init_globals */
static PHP_GINIT_FUNCTION(intl)
{
#if defined(COMPILE_DL_INTL) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset( intl_globals, 0, sizeof(zend_intl_globals) );
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION( intl )
{
	/* For the default locale php.ini setting */
	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("INTL_MAX_LOCALE_LEN", INTL_MAX_LOCALE_LEN, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("INTL_ICU_VERSION", U_ICU_VERSION, CONST_PERSISTENT | CONST_CS);
#ifdef U_ICU_DATA_VERSION
	REGISTER_STRING_CONSTANT("INTL_ICU_DATA_VERSION", U_ICU_DATA_VERSION, CONST_PERSISTENT | CONST_CS);
#endif

	/* Register 'Collator' PHP class */
	collator_register_Collator_class(  );

	/* Expose Collator constants to PHP scripts */
	collator_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'NumberFormatter' PHP class */
	formatter_register_class(  );

	/* Expose NumberFormatter constants to PHP scripts */
	formatter_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'Normalizer' PHP class */
	normalizer_register_Normalizer_class(  );

	/* Expose Normalizer constants to PHP scripts */
	normalizer_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'Locale' PHP class */
	locale_register_Locale_class(  );

	/* Expose Locale constants to PHP scripts */
	locale_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	msgformat_register_class();

	grapheme_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'DateFormat' PHP class */
	dateformat_register_IntlDateFormatter_class(  );

	/* Expose DateFormat constants to PHP scripts */
	dateformat_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'ResourceBundle' PHP class */
	resourcebundle_register_class( );

	/* Register 'Transliterator' PHP class */
	transliterator_register_Transliterator_class(  );

	/* Register Transliterator constants */
	transliterator_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'IntlTimeZone' PHP class */
	timezone_register_IntlTimeZone_class(  );

	/* Register 'IntlCalendar' PHP class */
	calendar_register_IntlCalendar_class(  );

	/* Expose ICU error codes to PHP scripts. */
	intl_expose_icu_error_codes( INIT_FUNC_ARGS_PASSTHRU );

	/* Expose IDN constants to PHP scripts. */
	idn_register_constants(INIT_FUNC_ARGS_PASSTHRU);

	/* Register 'Spoofchecker' PHP class */
	spoofchecker_register_Spoofchecker_class(  );

	/* Expose Spoofchecker constants to PHP scripts */
	spoofchecker_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'IntlException' PHP class */
	intl_register_IntlException_class(  );

	/* Register 'IntlIterator' PHP class */
	intl_register_IntlIterator_class(  );

	/* Register 'BreakIterator' class */
	breakiterator_register_BreakIterator_class(  );

	/* Register 'IntlPartsIterator' class */
	breakiterator_register_IntlPartsIterator_class(  );

	/* Global error handling. */
	intl_error_init( NULL );

	/* 'Converter' class for codepage conversions */
	php_converter_minit(INIT_FUNC_ARGS_PASSTHRU);

	/* IntlChar class */
	php_uchar_minit(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

#define EXPLICIT_CLEANUP_ENV_VAR "INTL_EXPLICIT_CLEANUP"

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION( intl )
{
	const char *cleanup;
    /* For the default locale php.ini setting */
    UNREGISTER_INI_ENTRIES();

	cleanup = getenv(EXPLICIT_CLEANUP_ENV_VAR);
    if (cleanup != NULL && !(cleanup[0] == '0' && cleanup[1] == '\0')) {
		u_cleanup();
    }

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION( intl )
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION( intl )
{
	INTL_G(current_collator) = NULL;
	if (INTL_G(grapheme_iterator)) {
		grapheme_close_global_iterator(  );
		INTL_G(grapheme_iterator) = NULL;
	}

	intl_error_reset( NULL);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION( intl )
{
#ifndef UCONFIG_NO_FORMATTING
	UErrorCode status = U_ZERO_ERROR;
	const char *tzdata_ver = NULL;
#endif

	php_info_print_table_start();
	php_info_print_table_header( 2, "Internationalization support", "enabled" );
	php_info_print_table_row( 2, "ICU version", U_ICU_VERSION );
#ifdef U_ICU_DATA_VERSION
	php_info_print_table_row( 2, "ICU Data version", U_ICU_DATA_VERSION );
#endif
#ifndef UCONFIG_NO_FORMATTING
	tzdata_ver = ucal_getTZDataVersion(&status);
	if (U_ZERO_ERROR == status) {
		php_info_print_table_row( 2, "ICU TZData version", tzdata_ver);
	}
#endif
	php_info_print_table_row( 2, "ICU Unicode version", U_UNICODE_VERSION );
	php_info_print_table_end();

    /* For the default locale php.ini setting */
    DISPLAY_INI_ENTRIES() ;
}
/* }}} */
