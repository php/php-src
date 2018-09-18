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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/

#include <unicode/uloc.h>
#include "php_intl.h"
#include "intl_error.h"
#include "locale_class.h"
#include "locale_methods.h"
#include "locale.h"

zend_class_entry *Locale_ce_ptr = NULL;

/*
 * 'Locale' class registration structures & functions
 */

/* {{{ Locale methods arguments info */
/*
 *  NOTE: when modifying 'locale_XX_args' do not forget to modify
 *        approptiate 'locale_XX_args' for the procedural API!
 */

ZEND_BEGIN_ARG_INFO_EX( locale_0_args, 0, 0, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( locale_arg_locale, 0, 0, 1 )
	ZEND_ARG_INFO( 0, locale )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( locale_accept_from_http_args, 0, 0, 1 )
	ZEND_ARG_INFO( 0, header )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( locale_compose_args, 0, 0, 1 )
	ZEND_ARG_INFO( 0, subtags )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( locale_2_args, 0, 0, 1 )
	ZEND_ARG_INFO( 0, locale )
	ZEND_ARG_INFO( 0, in_locale )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( locale_filter_matches_args, 0, 0, 2 )
        ZEND_ARG_INFO( 0, langtag )
        ZEND_ARG_INFO( 0, locale )
        ZEND_ARG_INFO( 0, canonicalize )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( locale_lookup_args, 0, 0, 2 )
        ZEND_ARG_INFO( 0, langtag )
        ZEND_ARG_INFO( 0, locale )
        ZEND_ARG_INFO( 0, canonicalize )
        ZEND_ARG_INFO( 0, default )
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ Locale_class_functions
 * Every 'Locale' class method has an entry in this table
 */

static const zend_function_entry Locale_class_functions[] = {
	ZEND_FENTRY( getDefault, zif_locale_get_default , locale_0_args , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC  )
	ZEND_FENTRY( setDefault, zif_locale_set_default , locale_arg_locale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getPrimaryLanguage, ZEND_FN( locale_get_primary_language ), locale_arg_locale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getScript, ZEND_FN( locale_get_script ), locale_arg_locale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getRegion, ZEND_FN( locale_get_region ), locale_arg_locale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getKeywords, ZEND_FN( locale_get_keywords ), locale_arg_locale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayScript, ZEND_FN( locale_get_display_script ), locale_2_args , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayRegion, ZEND_FN( locale_get_display_region ), locale_2_args , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayName, ZEND_FN( locale_get_display_name ), locale_2_args , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayLanguage, ZEND_FN( locale_get_display_language ), locale_2_args , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayVariant, ZEND_FN( locale_get_display_variant ), locale_2_args , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( composeLocale, ZEND_FN( locale_compose ), locale_compose_args , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( parseLocale, ZEND_FN( locale_parse ), locale_arg_locale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getAllVariants, ZEND_FN( locale_get_all_variants ), locale_arg_locale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( filterMatches, ZEND_FN( locale_filter_matches ), locale_filter_matches_args, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( lookup, ZEND_FN( locale_lookup ), locale_lookup_args, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( canonicalize, ZEND_FN( locale_canonicalize ), locale_arg_locale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( acceptFromHttp, ZEND_FN( locale_accept_from_http ), locale_accept_from_http_args , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_FE_END
};
/* }}} */

/* {{{ locale_register_Locale_class
 * Initialize 'Locale' class
 */
void locale_register_Locale_class( void )
{
	zend_class_entry ce;

	/* Create and register 'Locale' class. */
	INIT_CLASS_ENTRY( ce, "Locale", Locale_class_functions );
	ce.create_object = NULL;
	Locale_ce_ptr = zend_register_internal_class( &ce );

	/* Declare 'Locale' class properties. */
	if( !Locale_ce_ptr )
	{
		zend_error( E_ERROR,
			"Locale: Failed to register Locale class.");
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
