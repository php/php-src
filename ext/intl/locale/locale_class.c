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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/

#include <unicode/uloc.h>
#include "php_intl.h"
#include "intl_error.h"
#include "locale_class.h"
#include "locale_methods.h"
#include "locale.h"
#include "locale_arginfo.h"

zend_class_entry *Locale_ce_ptr = NULL;

/*
 * 'Locale' class registration structures & functions
 */

/* {{{ Locale_class_functions
 * Every 'Locale' class method has an entry in this table
 */

static const zend_function_entry Locale_class_functions[] = {
	ZEND_FENTRY( getDefault, zif_locale_get_default , arginfo_class_Locale_getDefault , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC  )
	ZEND_FENTRY( setDefault, zif_locale_set_default , arginfo_class_Locale_setDefault , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getPrimaryLanguage, ZEND_FN( locale_get_primary_language ), arginfo_class_Locale_getPrimaryLanguage , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getScript, ZEND_FN( locale_get_script ), arginfo_class_Locale_getScript , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getRegion, ZEND_FN( locale_get_region ), arginfo_class_Locale_getRegion , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getKeywords, ZEND_FN( locale_get_keywords ), arginfo_class_Locale_getKeywords , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayScript, ZEND_FN( locale_get_display_script ), arginfo_class_Locale_getDisplayScript , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayRegion, ZEND_FN( locale_get_display_region ), arginfo_class_Locale_getDisplayRegion , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayName, ZEND_FN( locale_get_display_name ), arginfo_class_Locale_getDisplayName , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayLanguage, ZEND_FN( locale_get_display_language ), arginfo_class_Locale_getDisplayLanguage , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getDisplayVariant, ZEND_FN( locale_get_display_variant ), arginfo_class_Locale_getDisplayVariant , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( composeLocale, ZEND_FN( locale_compose ), arginfo_class_Locale_composeLocale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( parseLocale, ZEND_FN( locale_parse ), arginfo_class_Locale_parseLocale , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( getAllVariants, ZEND_FN( locale_get_all_variants ), arginfo_class_Locale_getAllVariants , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( filterMatches, ZEND_FN( locale_filter_matches ), arginfo_class_Locale_filterMatches, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( lookup, ZEND_FN( locale_lookup ), arginfo_class_Locale_lookup, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( canonicalize, ZEND_FN( locale_canonicalize ), arginfo_class_Locale_canonicalize , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	ZEND_FENTRY( acceptFromHttp, ZEND_FN( locale_accept_from_http ), arginfo_class_Locale_acceptFromHttp , ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
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
