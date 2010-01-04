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
#include "intl_error.h"
#include "collator/collator_class.h"
#include "collator/collator.h"
#include "collator/collator_attr.h"
#include "collator/collator_compare.h"
#include "collator/collator_sort.h"
#include "collator/collator_convert.h"
#include "collator/collator_locale.h"
#include "collator/collator_create.h"
#include "collator/collator_error.h"

#include "formatter/formatter.h"
#include "formatter/formatter_class.h"
#include "formatter/formatter_attr.h"
#include "formatter/formatter_format.h"
#include "formatter/formatter_main.h"
#include "formatter/formatter_parse.h"

#include "msgformat/msgformat.h"
#include "msgformat/msgformat_class.h"
#include "msgformat/msgformat_attr.h"
#include "msgformat/msgformat_format.h"
#include "msgformat/msgformat_parse.h"

#include "normalizer/normalizer.h"
#include "normalizer/normalizer_class.h"
#include "normalizer/normalizer_normalize.h"

#include "locale/locale.h"
#include "locale/locale_class.h"
#include "locale/locale_methods.h"

#include "dateformat/dateformat.h"
#include "dateformat/dateformat_class.h"
#include "dateformat/dateformat_attr.h"
#include "dateformat/dateformat_format.h"
#include "dateformat/dateformat_parse.h"
#include "dateformat/dateformat_data.h"

#include "resourcebundle/resourcebundle_class.h"

#include "idn/idn.h"

#include "msgformat/msgformat.h"
#include "common/common_error.h"

#include <unicode/uloc.h>
#include <ext/standard/info.h>

#include "php_ini.h"
#define INTL_MODULE_VERSION PHP_INTL_VERSION

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

/* {{{ Arguments info */
ZEND_BEGIN_ARG_INFO_EX(collator_static_0_args, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_static_1_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, arg1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_static_2_args, 0, 0, 2)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_0_args, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_1_arg, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_INFO(0, arg1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_2_args, 0, 0, 3)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_sort_args, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_ARRAY_INFO(1, arr, 0)
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(numfmt_parse_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, formatter)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(numfmt_parse_currency_arginfo, 0, 0, 3)
	ZEND_ARG_INFO(0, formatter)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, currency)
	ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_0_args, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_1_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, arg1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_2_args, 0, 0, 2)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_3_args, 0, 0, 3)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, arg3)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_4_args, 0, 0, 4)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, arg3)
	ZEND_ARG_INFO(0, arg4)
ZEND_END_ARG_INFO()

#define intl_0_args collator_static_0_args
#define intl_1_arg collator_static_1_arg

ZEND_BEGIN_ARG_INFO_EX(normalizer_args, 0, 0, 1)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, form)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_1_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_search_args, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_substr_args, 0, 0, 2)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_strstr_args, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, before_needle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_extract_args, 0, 0, 2)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, arg3)
	ZEND_ARG_INFO(0, arg4)
	ZEND_ARG_INFO(1, arg5)  /* 1 = pass by reference */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(datefmt_parse_args, 0, 0, 2)
	ZEND_ARG_INFO(0, formatter)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_create, 0, 0, 2)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, style)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_get_error_code, 0, 0, 1)
	ZEND_ARG_INFO(0, nf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_format, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, num)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_format_currency, 0, 0, 3)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, num)
	ZEND_ARG_INFO(0, currency)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_get_attribute, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, attr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_set_attribute, 0, 0, 3)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, attr)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_set_symbol, 0, 0, 3)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, attr)
	ZEND_ARG_INFO(0, symbol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_set_pattern, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_get_locale, 0, 0, 1)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_create, 0, 0, 2)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_get_error_code, 0, 0, 1)
	ZEND_ARG_INFO(0, nf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_get_error_message, 0, 0, 1)
	ZEND_ARG_INFO(0, coll)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_format, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_format_message, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_parse, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_parse_message, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_set_pattern, 0, 0, 2)
	ZEND_ARG_INFO(0, mf)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_get_locale, 0, 0, 1)
	ZEND_ARG_INFO(0, mf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_set_pattern, 0, 0, 2)
	ZEND_ARG_INFO(0, mf)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_set_calendar, 0, 0, 2)
	ZEND_ARG_INFO(0, mf)
	ZEND_ARG_INFO(0, calendar)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_format, 0, 0, 0)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_create, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, date_type)
	ZEND_ARG_INFO(0, time_type)
	ZEND_ARG_INFO(0, timezone_str)
	ZEND_ARG_INFO(0, calendar)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_idn_to_ascii, 0, 0, 1)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, status)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_idn_to_utf8, 0, 0, 1)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, status)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_create_proc, 0, 0, 2 )
	ZEND_ARG_INFO( 0, locale )
	ZEND_ARG_INFO( 0, bundlename )
	ZEND_ARG_INFO( 0, fallback )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_get_proc, 0, 0, 2 )
    ZEND_ARG_INFO( 0, bundle )
	ZEND_ARG_INFO( 0, index )
	ZEND_ARG_INFO( 0, fallback )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_count_proc, 0, 0, 1 )
  ZEND_ARG_INFO( 0, bundle )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_locales_proc, 0, 0, 1 )
	ZEND_ARG_INFO( 0, bundlename )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_get_error_code_proc, 0, 0, 1 )
  ZEND_ARG_INFO( 0, bundle )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_get_error_message_proc, 0, 0, 1 )
  ZEND_ARG_INFO( 0, bundle )
ZEND_END_ARG_INFO()


/* }}} */

/* {{{ intl_functions
 *
 * Every user visible function must have an entry in intl_functions[].
 */
zend_function_entry intl_functions[] = {

	/* collator functions */
	PHP_FE( collator_create, collator_static_1_arg )
	PHP_FE( collator_compare, collator_2_args )
	PHP_FE( collator_get_attribute, collator_1_arg )
	PHP_FE( collator_set_attribute, collator_2_args )
	PHP_FE( collator_get_strength, collator_0_args )
	PHP_FE( collator_set_strength, collator_1_arg )
	PHP_FE( collator_sort, collator_sort_args )
	PHP_FE( collator_sort_with_sort_keys, collator_sort_args )
	PHP_FE( collator_asort, collator_sort_args )
	PHP_FE( collator_get_locale, collator_1_arg )
	PHP_FE( collator_get_error_code, collator_0_args )
	PHP_FE( collator_get_error_message, collator_0_args )
	PHP_FE( collator_get_sort_key, collator_2_args )

	/* formatter functions */
	PHP_FE( numfmt_create, arginfo_numfmt_create )
	PHP_FE( numfmt_format, arginfo_numfmt_format )
	PHP_FE( numfmt_parse, numfmt_parse_arginfo )
	PHP_FE( numfmt_format_currency, arginfo_numfmt_format_currency )
	PHP_FE( numfmt_parse_currency, numfmt_parse_currency_arginfo )
	PHP_FE( numfmt_set_attribute, arginfo_numfmt_set_attribute )
	PHP_FE( numfmt_get_attribute, arginfo_numfmt_get_attribute )
	PHP_FE( numfmt_set_text_attribute, arginfo_numfmt_set_attribute )
	PHP_FE( numfmt_get_text_attribute, arginfo_numfmt_get_attribute )
	PHP_FE( numfmt_set_symbol, arginfo_numfmt_set_symbol )
	PHP_FE( numfmt_get_symbol, arginfo_numfmt_get_attribute )
	PHP_FE( numfmt_set_pattern, arginfo_numfmt_set_pattern )
	PHP_FE( numfmt_get_pattern, arginfo_numfmt_get_error_code )
	PHP_FE( numfmt_get_locale, arginfo_numfmt_get_locale )
	PHP_FE( numfmt_get_error_code, arginfo_numfmt_get_error_code )
	PHP_FE( numfmt_get_error_message, arginfo_numfmt_get_error_code )

	/* normalizer functions */
	PHP_FE( normalizer_normalize, normalizer_args )
	PHP_FE( normalizer_is_normalized, normalizer_args )

	/* Locale functions */
	PHP_NAMED_FE( locale_get_default, zif_locale_get_default, locale_0_args )
	PHP_NAMED_FE( locale_set_default, zif_locale_set_default, locale_1_arg )
	PHP_FE( locale_get_primary_language, locale_1_arg )
	PHP_FE( locale_get_script, locale_1_arg )
	PHP_FE( locale_get_region, locale_1_arg )
	PHP_FE( locale_get_keywords, locale_1_arg )
	PHP_FE( locale_get_display_script, locale_2_args )
	PHP_FE( locale_get_display_region, locale_2_args )
	PHP_FE( locale_get_display_name, locale_2_args )
	PHP_FE( locale_get_display_language, locale_2_args)
	PHP_FE( locale_get_display_variant, locale_2_args )
	PHP_FE( locale_compose, locale_1_arg )
	PHP_FE( locale_parse, locale_1_arg )
	PHP_FE( locale_get_all_variants, locale_1_arg )
	PHP_FE( locale_filter_matches, locale_3_args )
	PHP_FE( locale_canonicalize, locale_1_arg )
	PHP_FE( locale_lookup, locale_4_args )
	PHP_FE( locale_accept_from_http, locale_1_arg )

	/* MessageFormatter functions */
	PHP_FE( msgfmt_create, arginfo_msgfmt_create )
	PHP_FE( msgfmt_format, arginfo_msgfmt_format )
	PHP_FE( msgfmt_format_message, arginfo_msgfmt_format_message )
	PHP_FE( msgfmt_parse, arginfo_msgfmt_parse )
	PHP_FE( msgfmt_parse_message, arginfo_numfmt_parse_message )
	PHP_FE( msgfmt_set_pattern, arginfo_msgfmt_set_pattern )
	PHP_FE( msgfmt_get_pattern, arginfo_msgfmt_get_locale )
	PHP_FE( msgfmt_get_locale, arginfo_msgfmt_get_locale )
	PHP_FE( msgfmt_get_error_code, arginfo_msgfmt_get_error_code )
	PHP_FE( msgfmt_get_error_message, arginfo_msgfmt_get_error_message )

	/* IntlDateFormatter functions */
	PHP_FE( datefmt_create, arginfo_datefmt_create )
	PHP_FE( datefmt_get_datetype, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_get_timetype, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_get_calendar, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_set_calendar, arginfo_datefmt_set_calendar )
	PHP_FE( datefmt_get_locale, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_get_timezone_id, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_set_timezone_id, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_get_pattern, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_set_pattern, arginfo_datefmt_set_pattern )
	PHP_FE( datefmt_is_lenient, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_set_lenient, arginfo_msgfmt_get_locale )
	PHP_FE( datefmt_format, arginfo_datefmt_format )
	PHP_FE( datefmt_parse, datefmt_parse_args )
	PHP_FE( datefmt_localtime , datefmt_parse_args )
	PHP_FE( datefmt_get_error_code, arginfo_msgfmt_get_error_code )
	PHP_FE( datefmt_get_error_message, arginfo_msgfmt_get_error_message )

	/* grapheme functions */
	PHP_FE( grapheme_strlen, grapheme_1_arg )
	PHP_FE( grapheme_strpos, grapheme_search_args )
	PHP_FE( grapheme_stripos, grapheme_search_args )
	PHP_FE( grapheme_strrpos, grapheme_search_args )
	PHP_FE( grapheme_strripos, grapheme_search_args )
	PHP_FE( grapheme_substr, grapheme_substr_args )
	PHP_FE( grapheme_strstr, grapheme_strstr_args )
	PHP_FE( grapheme_stristr, grapheme_strstr_args )
	PHP_FE( grapheme_extract, grapheme_extract_args )

	/* IDN functions */
	PHP_FE( idn_to_ascii, arginfo_idn_to_ascii)
	PHP_FE( idn_to_utf8, arginfo_idn_to_ascii)

	/* ResourceBundle functions */
	PHP_FE( resourcebundle_create, arginfo_resourcebundle_create_proc )
	PHP_FE( resourcebundle_get, arginfo_resourcebundle_get_proc )
	PHP_FE( resourcebundle_count, arginfo_resourcebundle_count_proc )
	PHP_FE( resourcebundle_locales, arginfo_resourcebundle_locales_proc )
	PHP_FE( resourcebundle_get_error_code, arginfo_resourcebundle_get_error_code_proc )
	PHP_FE( resourcebundle_get_error_message, arginfo_resourcebundle_get_error_message_proc )

	/* common functions */
	PHP_FE( intl_get_error_code, intl_0_args )
	PHP_FE( intl_get_error_message, intl_0_args )
	PHP_FE( intl_is_failure, intl_1_arg )
	PHP_FE( intl_error_name, intl_1_arg )

	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ INI Settings */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY(LOCALE_INI_NAME, NULL, PHP_INI_ALL, OnUpdateStringUnempty, default_locale, zend_intl_globals, intl_globals)
    STD_PHP_INI_ENTRY("intl.error_level", "0", PHP_INI_ALL, OnUpdateLong, error_level, zend_intl_globals, intl_globals)

PHP_INI_END()
/* }}} */


static PHP_GINIT_FUNCTION(intl);

/* {{{ intl_module_entry */
zend_module_entry intl_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"intl",
	intl_functions,
	PHP_MINIT( intl ),
	PHP_MSHUTDOWN( intl ),
	PHP_RINIT( intl ),
	PHP_RSHUTDOWN( intl ),
	PHP_MINFO( intl ),
	INTL_MODULE_VERSION,
	PHP_MODULE_GLOBALS(intl),   /* globals descriptor */
	PHP_GINIT(intl),            /* globals ctor */
	NULL,                       /* globals dtor */
	NULL,                       /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_INTL
ZEND_GET_MODULE( intl )
#endif

/* {{{ intl_init_globals */
static PHP_GINIT_FUNCTION(intl)
{
	memset( intl_globals, 0, sizeof(zend_intl_globals) );
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION( intl )
{
	/* For the default locale php.ini setting */
	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("INTL_MAX_LOCALE_LEN", INTL_MAX_LOCALE_LEN, CONST_CS);

	/* Register 'Collator' PHP class */
	collator_register_Collator_class( TSRMLS_C );

	/* Expose Collator constants to PHP scripts */
	collator_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'NumberFormatter' PHP class */
	formatter_register_class( TSRMLS_C );

	/* Expose NumberFormatter constants to PHP scripts */
	formatter_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'Normalizer' PHP class */
	normalizer_register_Normalizer_class( TSRMLS_C );

	/* Expose Normalizer constants to PHP scripts */
	normalizer_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'Locale' PHP class */
	locale_register_Locale_class( TSRMLS_C );

	/* Expose Locale constants to PHP scripts */
	locale_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	msgformat_register_class(TSRMLS_C);

	grapheme_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'DateFormat' PHP class */
	dateformat_register_IntlDateFormatter_class( TSRMLS_C );

	/* Expose DateFormat constants to PHP scripts */
	dateformat_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'ResourceBundle' PHP class */
	resourcebundle_register_class( TSRMLS_C);

	/* Expose ICU error codes to PHP scripts. */
	intl_expose_icu_error_codes( INIT_FUNC_ARGS_PASSTHRU );

	/* Expose IDN constants to PHP scripts. */
	idn_register_constants(INIT_FUNC_ARGS_PASSTHRU);

	/* Global error handling. */
	intl_error_init( NULL TSRMLS_CC );

	/* Set the default_locale value */
	if( INTL_G(default_locale) == NULL ) {
		INTL_G(default_locale) = pestrdup(uloc_getDefault(), 1) ;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION( intl )
{
    /* For the default locale php.ini setting */
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION( intl )
{
	/* Set the default_locale value */
    if( INTL_G(default_locale) == NULL ) {
        INTL_G(default_locale) = pestrdup(uloc_getDefault(), 1) ;
    }
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION( intl )
{
	if(INTL_G(current_collator)) {
		INTL_G(current_collator) = NULL;
	}
	if (INTL_G(grapheme_iterator)) {
		grapheme_close_global_iterator( TSRMLS_C );
		INTL_G(grapheme_iterator) = NULL;
	}

	intl_error_reset( NULL TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION( intl )
{
	php_info_print_table_start();
	php_info_print_table_header( 2, "Internationalization support", "enabled" );
	php_info_print_table_row( 2, "version", INTL_MODULE_VERSION );
	php_info_print_table_row( 2, "ICU version", U_ICU_VERSION );
	php_info_print_table_end();

    /* For the default locale php.ini setting */
    DISPLAY_INI_ENTRIES() ;
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
