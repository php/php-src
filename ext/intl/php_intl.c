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
#include "collator/collator_attr.h"
#include "collator/collator_compare.h"
#include "collator/collator_sort.h"
#include "collator/collator_convert.h"
#include "collator/collator_locale.h"
#include "collator/collator_create.h"
#include "collator/collator_error.h"

#include "converter/converter.h"

#include "formatter/formatter.h"
#include "formatter/formatter_class.h"
#include "formatter/formatter_attr.h"
#include "formatter/formatter_format.h"
#include "formatter/formatter_main.h"
#include "formatter/formatter_parse.h"

#include "grapheme/grapheme.h"

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
#include "dateformat/dateformat_attrcpp.h"
#include "dateformat/dateformat_format.h"
#include "dateformat/dateformat_format_object.h"
#include "dateformat/dateformat_parse.h"
#include "dateformat/dateformat_data.h"

#include "resourcebundle/resourcebundle_class.h"

#include "transliterator/transliterator.h"
#include "transliterator/transliterator_class.h"
#include "transliterator/transliterator_methods.h"

#include "timezone/timezone_class.h"
#include "timezone/timezone_methods.h"

#include "calendar/calendar_class.h"
#include "calendar/calendar_methods.h"
#include "calendar/gregoriancalendar_methods.h"

#include "breakiterator/breakiterator_class.h"
#include "breakiterator/breakiterator_iterators.h"

#include "idn/idn.h"
#include "uchar/uchar.h"

# include "spoofchecker/spoofchecker_class.h"
# include "spoofchecker/spoofchecker.h"
# include "spoofchecker/spoofchecker_create.h"
# include "spoofchecker/spoofchecker_main.h"

#include "msgformat/msgformat.h"
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

/* {{{ intl_functions
 *
 * Every user visible function must have an entry in intl_functions[].
 */
static const zend_function_entry intl_functions[] = {

	/* collator functions */
	PHP_FE( collator_create, arginfo_collator_create )
	PHP_FE( collator_compare, arginfo_collator_compare )
	PHP_FE( collator_get_attribute, arginfo_collator_get_attribute )
	PHP_FE( collator_set_attribute, arginfo_collator_set_attribute )
	PHP_FE( collator_get_strength, arginfo_collator_get_strength )
	PHP_FE( collator_set_strength, arginfo_collator_set_strength )
	PHP_FE( collator_sort, arginfo_collator_sort )
	PHP_FE( collator_sort_with_sort_keys, arginfo_collator_sort_with_sort_keys )
	PHP_FE( collator_asort, arginfo_collator_asort )
	PHP_FE( collator_get_locale, arginfo_collator_get_locale )
	PHP_FE( collator_get_error_code, arginfo_collator_get_error_code )
	PHP_FE( collator_get_error_message, arginfo_collator_get_error_message )
	PHP_FE( collator_get_sort_key, arginfo_collator_get_sort_key )

	/* formatter functions */
	PHP_FE( numfmt_create, arginfo_numfmt_create )
	PHP_FE( numfmt_format, arginfo_numfmt_format )
	PHP_FE( numfmt_parse, arginfo_numfmt_parse )
	PHP_FE( numfmt_format_currency, arginfo_numfmt_format_currency )
	PHP_FE( numfmt_parse_currency, arginfo_numfmt_parse_currency )
	PHP_FE( numfmt_set_attribute, arginfo_numfmt_set_attribute )
	PHP_FE( numfmt_get_attribute, arginfo_numfmt_get_attribute )
	PHP_FE( numfmt_set_text_attribute, arginfo_numfmt_set_text_attribute )
	PHP_FE( numfmt_get_text_attribute, arginfo_numfmt_get_text_attribute )
	PHP_FE( numfmt_set_symbol, arginfo_numfmt_set_symbol )
	PHP_FE( numfmt_get_symbol, arginfo_numfmt_get_symbol )
	PHP_FE( numfmt_set_pattern, arginfo_numfmt_set_pattern )
	PHP_FE( numfmt_get_pattern, arginfo_numfmt_get_pattern )
	PHP_FE( numfmt_get_locale, arginfo_numfmt_get_locale )
	PHP_FE( numfmt_get_error_code, arginfo_numfmt_get_error_code )
	PHP_FE( numfmt_get_error_message, arginfo_numfmt_get_error_message )

	/* normalizer functions */
	PHP_FE( normalizer_normalize, arginfo_normalizer_normalize )
	PHP_FE( normalizer_is_normalized, arginfo_normalizer_is_normalized )
#if U_ICU_VERSION_MAJOR_NUM >= 56
	PHP_FE( normalizer_get_raw_decomposition, arginfo_normalizer_get_raw_decomposition )
#endif

	/* Locale functions */
	PHP_NAMED_FE( locale_get_default, zif_locale_get_default, arginfo_locale_get_default )
	PHP_NAMED_FE( locale_set_default, zif_locale_set_default, arginfo_locale_set_default )
	PHP_FE( locale_get_primary_language, arginfo_locale_get_primary_language )
	PHP_FE( locale_get_script, arginfo_locale_get_script )
	PHP_FE( locale_get_region, arginfo_locale_get_region )
	PHP_FE( locale_get_keywords, arginfo_locale_get_keywords )
	PHP_FE( locale_get_display_script, arginfo_locale_get_display_script )
	PHP_FE( locale_get_display_region, arginfo_locale_get_display_region )
	PHP_FE( locale_get_display_name, arginfo_locale_get_display_name )
	PHP_FE( locale_get_display_language, arginfo_locale_get_display_language)
	PHP_FE( locale_get_display_variant, arginfo_locale_get_display_variant )
	PHP_FE( locale_compose, arginfo_locale_compose )
	PHP_FE( locale_parse, arginfo_locale_parse )
	PHP_FE( locale_get_all_variants, arginfo_locale_get_all_variants )
	PHP_FE( locale_filter_matches, arginfo_locale_filter_matches )
	PHP_FE( locale_canonicalize, arginfo_locale_canonicalize )
	PHP_FE( locale_lookup, arginfo_locale_lookup )
	PHP_FE( locale_accept_from_http, arginfo_locale_accept_from_http )

	/* MessageFormatter functions */
	PHP_FE( msgfmt_create, arginfo_msgfmt_create )
	PHP_FE( msgfmt_format, arginfo_msgfmt_format )
	PHP_FE( msgfmt_format_message, arginfo_msgfmt_format_message )
	PHP_FE( msgfmt_parse, arginfo_msgfmt_parse )
	PHP_FE( msgfmt_parse_message, arginfo_msgfmt_parse_message )
	PHP_FE( msgfmt_set_pattern, arginfo_msgfmt_set_pattern )
	PHP_FE( msgfmt_get_pattern, arginfo_msgfmt_get_locale )
	PHP_FE( msgfmt_get_locale, arginfo_msgfmt_get_locale )
	PHP_FE( msgfmt_get_error_code, arginfo_msgfmt_get_error_code )
	PHP_FE( msgfmt_get_error_message, arginfo_msgfmt_get_error_message )

	/* IntlDateFormatter functions */
	PHP_FE( datefmt_create, arginfo_datefmt_create )
	PHP_FE( datefmt_get_datetype, arginfo_datefmt_get_datetype )
	PHP_FE( datefmt_get_timetype, arginfo_datefmt_get_timetype )
	PHP_FE( datefmt_get_calendar, arginfo_datefmt_get_calendar )
	PHP_FE( datefmt_get_calendar_object, arginfo_datefmt_get_calendar_object )
	PHP_FE( datefmt_set_calendar, arginfo_datefmt_set_calendar )
	PHP_FE( datefmt_get_locale, arginfo_datefmt_get_locale )
	PHP_FE( datefmt_get_timezone_id, arginfo_datefmt_get_timezone_id )
	PHP_FE( datefmt_get_timezone, arginfo_datefmt_get_timezone )
	PHP_FE( datefmt_set_timezone, arginfo_datefmt_set_timezone )
	PHP_FE( datefmt_get_pattern, arginfo_datefmt_get_pattern )
	PHP_FE( datefmt_set_pattern, arginfo_datefmt_set_pattern )
	PHP_FE( datefmt_is_lenient, arginfo_datefmt_is_lenient )
	PHP_FE( datefmt_set_lenient, arginfo_datefmt_set_lenient )
	PHP_FE( datefmt_format, arginfo_datefmt_format )
	PHP_FE( datefmt_format_object, arginfo_datefmt_format_object )
	PHP_FE( datefmt_parse, arginfo_datefmt_parse )
	PHP_FE( datefmt_localtime , arginfo_datefmt_localtime )
	PHP_FE( datefmt_get_error_code, arginfo_datefmt_get_error_code )
	PHP_FE( datefmt_get_error_message, arginfo_datefmt_get_error_message )

	/* grapheme functions */
	PHP_FE( grapheme_strlen, arginfo_grapheme_strlen )
	PHP_FE( grapheme_strpos, arginfo_grapheme_strpos )
	PHP_FE( grapheme_stripos, arginfo_grapheme_stripos )
	PHP_FE( grapheme_strrpos, arginfo_grapheme_strrpos )
	PHP_FE( grapheme_strripos, arginfo_grapheme_strripos )
	PHP_FE( grapheme_substr, arginfo_grapheme_substr )
	PHP_FE( grapheme_strstr, arginfo_grapheme_strstr )
	PHP_FE( grapheme_stristr, arginfo_grapheme_stristr )
	PHP_FE( grapheme_extract, arginfo_grapheme_extract )

	/* IDN functions */
	PHP_FE( idn_to_ascii, arginfo_idn_to_ascii)
	PHP_FE( idn_to_utf8, arginfo_idn_to_utf8)

	/* ResourceBundle functions */
	PHP_FE( resourcebundle_create, arginfo_resourcebundle_create )
	PHP_FE( resourcebundle_get, arginfo_resourcebundle_get )
	PHP_FE( resourcebundle_count, arginfo_resourcebundle_count )
	PHP_FE( resourcebundle_locales, arginfo_resourcebundle_locales )
	PHP_FE( resourcebundle_get_error_code, arginfo_resourcebundle_get_error_code )
	PHP_FE( resourcebundle_get_error_message, arginfo_resourcebundle_get_error_message )

	/* Transliterator functions */
	PHP_FE( transliterator_create, arginfo_transliterator_create )
	PHP_FE( transliterator_create_from_rules, arginfo_transliterator_create_from_rules )
	PHP_FE( transliterator_list_ids, arginfo_transliterator_list_ids )
	PHP_FE( transliterator_create_inverse, arginfo_transliterator_create_inverse)
	PHP_FE( transliterator_transliterate, arginfo_transliterator_transliterate )
	PHP_FE( transliterator_get_error_code, arginfo_transliterator_get_error_code )
	PHP_FE( transliterator_get_error_message, arginfo_transliterator_get_error_message )

	/* TimeZone functions */
	PHP_FE( intltz_create_time_zone, arginfo_intltz_create_time_zone )
	PHP_FE( intltz_from_date_time_zone, arginfo_intltz_from_date_time_zone )
	PHP_FE( intltz_create_default, arginfo_intltz_create_default )
	PHP_FE( intltz_get_id, arginfo_intltz_get_id )
	PHP_FE( intltz_get_gmt, arginfo_intltz_get_gmt )
	PHP_FE( intltz_get_unknown, arginfo_intltz_get_unknown )
	PHP_FE( intltz_create_enumeration, arginfo_intltz_create_enumeration )
	PHP_FE( intltz_count_equivalent_ids, arginfo_intltz_count_equivalent_ids )
	PHP_FE( intltz_create_time_zone_id_enumeration, arginfo_intltz_create_time_zone_id_enumeration )
	PHP_FE( intltz_get_canonical_id, arginfo_intltz_get_canonical_id )
	PHP_FE( intltz_get_region, arginfo_intltz_get_region )
	PHP_FE( intltz_get_tz_data_version, arginfo_intltz_get_tz_data_version )
	PHP_FE( intltz_get_equivalent_id, arginfo_intltz_get_equivalent_id )
	PHP_FE( intltz_use_daylight_time, arginfo_intltz_use_daylight_time )
	PHP_FE( intltz_get_offset, arginfo_intltz_get_offset )
	PHP_FE( intltz_get_raw_offset, arginfo_intltz_get_raw_offset )
	PHP_FE( intltz_has_same_rules, arginfo_intltz_has_same_rules )
	PHP_FE( intltz_get_display_name, arginfo_intltz_get_display_name )
	PHP_FE( intltz_get_dst_savings, arginfo_intltz_get_dst_savings )
	PHP_FE( intltz_to_date_time_zone, arginfo_intltz_to_date_time_zone )
	PHP_FE( intltz_get_error_code, arginfo_intltz_get_error_code )
	PHP_FE( intltz_get_error_message, arginfo_intltz_get_error_message )

	PHP_FE( intlcal_create_instance, arginfo_intlcal_create_instance )
	PHP_FE( intlcal_get_keyword_values_for_locale, arginfo_intlcal_get_keyword_values_for_locale )
	PHP_FE( intlcal_get_now, arginfo_intlcal_get_now )
	PHP_FE( intlcal_get_available_locales, arginfo_intlcal_get_available_locales )
	PHP_FE( intlcal_get, arginfo_intlcal_get )
	PHP_FE( intlcal_get_time, arginfo_intlcal_get_time )
	PHP_FE( intlcal_set_time, arginfo_intlcal_set_time )
	PHP_FE( intlcal_add, arginfo_intlcal_add )
	PHP_FE( intlcal_set_time_zone, arginfo_intlcal_set_time_zone )
	PHP_FE( intlcal_after, arginfo_intlcal_after )
	PHP_FE( intlcal_before, arginfo_intlcal_before )
	PHP_FE( intlcal_set, arginfo_intlcal_set )
	PHP_FE( intlcal_roll, arginfo_intlcal_roll )
	PHP_FE( intlcal_clear, arginfo_intlcal_clear )
	PHP_FE( intlcal_field_difference, arginfo_intlcal_field_difference )
	PHP_FE( intlcal_get_actual_maximum, arginfo_intlcal_get_actual_maximum )
	PHP_FE( intlcal_get_actual_minimum, arginfo_intlcal_get_actual_minimum )
	PHP_FE( intlcal_get_day_of_week_type, arginfo_intlcal_get_day_of_week_type )
	PHP_FE( intlcal_get_first_day_of_week, arginfo_intlcal_get_first_day_of_week )
	PHP_FE( intlcal_get_greatest_minimum, arginfo_intlcal_get_greatest_minimum )
	PHP_FE( intlcal_get_least_maximum, arginfo_intlcal_get_least_maximum )
	PHP_FE( intlcal_get_locale, arginfo_intlcal_get_locale )
	PHP_FE( intlcal_get_maximum, arginfo_intlcal_get_maximum )
	PHP_FE( intlcal_get_minimal_days_in_first_week, arginfo_intlcal_get_minimal_days_in_first_week )
	PHP_FE( intlcal_get_minimum, arginfo_intlcal_get_minimum )
	PHP_FE( intlcal_get_time_zone, arginfo_intlcal_get_time_zone )
	PHP_FE( intlcal_get_type, arginfo_intlcal_get_type )
	PHP_FE( intlcal_get_weekend_transition, arginfo_intlcal_get_weekend_transition )
	PHP_FE( intlcal_in_daylight_time, arginfo_intlcal_in_daylight_time )
	PHP_FE( intlcal_is_equivalent_to, arginfo_intlcal_is_equivalent_to )
	PHP_FE( intlcal_is_lenient, arginfo_intlcal_is_lenient )
	PHP_FE( intlcal_is_set, arginfo_intlcal_is_set )
	PHP_FE( intlcal_is_weekend, arginfo_intlcal_is_weekend )
	PHP_FE( intlcal_set_first_day_of_week, arginfo_intlcal_set_first_day_of_week )
	PHP_FE( intlcal_set_lenient, arginfo_intlcal_set_lenient )
	PHP_FE( intlcal_set_minimal_days_in_first_week, arginfo_intlcal_set_minimal_days_in_first_week )
	PHP_FE( intlcal_equals, arginfo_intlcal_equals )
	PHP_FE( intlcal_from_date_time, arginfo_intlcal_from_date_time )
	PHP_FE( intlcal_to_date_time, arginfo_intlcal_to_date_time )
	PHP_FE( intlcal_get_repeated_wall_time_option, arginfo_intlcal_get_repeated_wall_time_option )
	PHP_FE( intlcal_get_skipped_wall_time_option, arginfo_intlcal_get_skipped_wall_time_option )
	PHP_FE( intlcal_set_repeated_wall_time_option, arginfo_intlcal_set_repeated_wall_time_option )
	PHP_FE( intlcal_set_skipped_wall_time_option, arginfo_intlcal_set_skipped_wall_time_option )
	PHP_FE( intlcal_get_error_code, arginfo_intlcal_get_error_code )
	PHP_FE( intlcal_get_error_message, arginfo_intlcal_get_error_message )

	PHP_FE( intlgregcal_create_instance, arginfo_intlgregcal_create_instance )
	PHP_FE( intlgregcal_set_gregorian_change, arginfo_intlgregcal_set_gregorian_change )
	PHP_FE( intlgregcal_get_gregorian_change, arginfo_intlgregcal_get_gregorian_change )
	PHP_FE( intlgregcal_is_leap_year, arginfo_intlgregcal_is_leap_year )

	/* common functions */
	PHP_FE( intl_get_error_code, arginfo_intl_get_error_code )
	PHP_FE( intl_get_error_message, arginfo_intl_get_error_message )
	PHP_FE( intl_is_failure, arginfo_intl_is_failure )
	PHP_FE( intl_error_name, arginfo_intl_error_name )

	PHP_FE_END
};
/* }}} */

/* {{{ INI Settings */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY(LOCALE_INI_NAME, NULL, PHP_INI_ALL, OnUpdateStringUnempty, default_locale, zend_intl_globals, intl_globals)
    STD_PHP_INI_ENTRY("intl.error_level", "0", PHP_INI_ALL, OnUpdateLong, error_level, zend_intl_globals, intl_globals)
	STD_PHP_INI_ENTRY("intl.use_exceptions", "0", PHP_INI_ALL, OnUpdateBool, use_exceptions, zend_intl_globals, intl_globals)
PHP_INI_END()
/* }}} */

static PHP_GINIT_FUNCTION(intl);

/* {{{ intl_module_entry */
zend_module_entry intl_module_entry = {
	STANDARD_MODULE_HEADER,
	"intl",
	intl_functions,
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

/* {{{ PHP_MINIT_FUNCTION
 */
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

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
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

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION( intl )
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION( intl )
{
	if(!Z_ISUNDEF(INTL_G(current_collator))) {
		ZVAL_UNDEF(&INTL_G(current_collator));
	}
	if (INTL_G(grapheme_iterator)) {
		grapheme_close_global_iterator(  );
		INTL_G(grapheme_iterator) = NULL;
	}

	intl_error_reset( NULL);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION( intl )
{
#if !UCONFIG_NO_FORMATTING
	UErrorCode status = U_ZERO_ERROR;
	const char *tzdata_ver = NULL;
#endif

	php_info_print_table_start();
	php_info_print_table_header( 2, "Internationalization support", "enabled" );
	php_info_print_table_row( 2, "ICU version", U_ICU_VERSION );
#ifdef U_ICU_DATA_VERSION
	php_info_print_table_row( 2, "ICU Data version", U_ICU_DATA_VERSION );
#endif
#if !UCONFIG_NO_FORMATTING
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
