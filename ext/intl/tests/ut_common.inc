<?php
/*
 * Run unit test in OO- and in procedural mode.
 * Then compare the outputs.
 * It they're equal then show one of them.
 * Otherwise indicate an error.
 */
function ut_run()
{
    // Run unit test in OO mode.
    $GLOBALS['oo-mode'] = true;
    $oo_result = ut_main();

    // Run unit test in procedural mode.
    $GLOBALS['oo-mode'] = false;
    $proc_result = ut_main();

    // Show error if the APIs produce different results.
    if( $proc_result !== $oo_result )
    {
          echo "ERROR: OO- and procedural APIs produce different results!\n";
          echo "OO API output:\n";
          echo str_repeat( '=', 78 ) . "\n";
          echo $oo_result;
          echo str_repeat( '=', 78 ) . "\n";
          echo "procedural API output:\n";
          echo str_repeat( '=', 78 ) . "\n";
          echo $proc_result;
          echo str_repeat( '=', 78 ) . "\n";
          return;
    }

    // Else, if the results are equal, show one of them.
    echo $proc_result;
}

function dump( $val )
{
    return var_export( $val, true );
}

/*
 * Wrappers around Collator methods to run them in either OO- or procedural mode.
 */

function ut_coll_create( $locale )
{
    return $GLOBALS['oo-mode'] ? Collator::create( $locale ) : collator_create( $locale );
}
function ut_coll_compare( $coll, $str1, $str2 )
{
    return $GLOBALS['oo-mode'] ? $coll->compare( $str1, $str2 ) : collator_compare( $coll, $str1, $str2 );
}
function ut_coll_sort( $coll, &$arr, $sort_flag = Collator::SORT_REGULAR )
{
    return $GLOBALS['oo-mode'] ? $coll->sort( $arr, $sort_flag ) : collator_sort( $coll, $arr, $sort_flag );
}
function ut_coll_sort_with_sort_keys( $coll, &$arr )
{
    return $GLOBALS['oo-mode'] ? $coll->sortWithSortKeys( $arr ) : collator_sort_with_sort_keys( $coll, $arr );
}
function ut_coll_get_sort_key( $coll, $str )
{
    return $GLOBALS['oo-mode'] ? $coll->getSortKey( $str ) : collator_get_sort_key( $coll, $str );
}
function ut_coll_asort( $coll, &$arr, $sort_flag = Collator::SORT_REGULAR )
{
    return $GLOBALS['oo-mode'] ? $coll->asort( $arr, $sort_flag ) : collator_asort( $coll, $arr, $sort_flag );
}
function ut_coll_get_locale( $coll, $type )
{
    return $GLOBALS['oo-mode'] ? $coll->getLocale( $type ) : collator_get_locale( $coll, $type );
}
function ut_coll_get_display_name( $obj_loc, $disp_loc )
{
    return $GLOBALS['oo-mode'] ? Collator::getDisplayName( $obj_loc, $disp_loc ) : collator_get_display_name( $obj_loc, $disp_loc );
}
function ut_coll_get_available_locales()
{
    return $GLOBALS['oo-mode'] ? Collator::getAvailableLocales() : collator_get_available_locales();
}
function ut_coll_get_attribute( $coll, $attr )
{
    return $GLOBALS['oo-mode'] ? $coll->getAttribute( $attr ) : collator_get_attribute( $coll, $attr );
}
function ut_coll_get_strength( $coll )
{
    return $GLOBALS['oo-mode'] ? $coll->getStrength() : collator_get_strength( $coll );
}
function ut_coll_set_strength( $coll, $strength )
{
    return $GLOBALS['oo-mode'] ? $coll->setStrength( $strength ) : collator_set_strength( $coll, $strength );
}
function ut_coll_set_attribute( $coll, $attr, $val )
{
    return $GLOBALS['oo-mode'] ? $coll->setAttribute( $attr, $val ) : collator_set_attribute( $coll, $attr, $val );
}
function ut_coll_get_variable_top( $coll )
{
    return $GLOBALS['oo-mode'] ? $coll->getVariableTop() : collator_get_variable_top( $coll );
}
function ut_coll_set_variable_top( $coll, $var_top )
{
    return $GLOBALS['oo-mode'] ? $coll->setVariableTop( $var_top ) : collator_set_variable_top( $coll, $var_top );
}
function ut_coll_restore_variable_top( $coll, $var_top )
{
    return $GLOBALS['oo-mode'] ? $coll->restoreVariableTop( $var_top ) : collator_restore_variable_top( $coll, $var_top );
}
function ut_coll_get_error_code( $coll )
{
    return $GLOBALS['oo-mode'] ? $coll->getErrorCode() : collator_get_error_code( $coll );
}
function ut_coll_get_error_message( $coll )
{
    return $GLOBALS['oo-mode'] ? $coll->getErrorMessage() : collator_get_error_message( $coll );
}
function ut_coll_get_default()
{
    return $GLOBALS['oo-mode'] ? Collator::getDefault() : collator_get_default();
}
function ut_coll_set_default( $coll )
{
    return $GLOBALS['oo-mode'] ? Collator::setDefault( $coll ) : collator_set_default( $coll );
}

/*
 * Wrappers around NumberFormatter methods to run them in either OO- or procedural mode.
 */

// FIXME: incomplete list

function ut_nfmt_create( $locale, $style, $pattern = null )
{
    return $GLOBALS['oo-mode'] ? new NumberFormatter( $locale, $style, $pattern ) : numfmt_create( $locale, $style, $pattern );
}
function ut_nfmt_format( $fmt, $number, $type = null )
{
    return $GLOBALS['oo-mode'] ? $fmt->format( $number, $type ) : numfmt_format( $fmt, $number, $type );
}
function ut_nfmt_parse( $fmt, $string, $type = NumberFormatter::TYPE_DOUBLE, &$position = null )
{
    if(is_null($position)) {
    return $GLOBALS['oo-mode'] ? $fmt->parse( $string, $type ) : numfmt_parse( $fmt, $string, $type );
    } else {
    return $GLOBALS['oo-mode'] ? $fmt->parse( $string, $type, $position ) : numfmt_parse( $fmt, $string, $type, $position );
    }
}
function ut_nfmt_format_currency( $fmt, $number, $currency )
{
    return $GLOBALS['oo-mode'] ? $fmt->formatCurrency( $number, $currency ) : numfmt_format_currency( $fmt, $number, $currency );
}
function ut_nfmt_parse_currency( $fmt, $string, &$currency, &$position = null )
{
    if(is_null($position)) {
    return $GLOBALS['oo-mode'] ? $fmt->parseCurrency( $string, $currency ) : numfmt_parse_currency( $fmt, $string, $currency );
    } else {
    return $GLOBALS['oo-mode'] ? $fmt->parseCurrency( $string, $currency, $position ) : numfmt_parse_currency( $fmt, $string, $currency, $position );
   }
}
function ut_nfmt_set_attribute( $fmt, $attribute, $value )
{
    return $GLOBALS['oo-mode'] ? $fmt->setAttribute( $attribute, $value ) : numfmt_set_attribute( $fmt, $attribute, $value );
}
function ut_nfmt_set_text_attribute( $fmt, $attribute, $value )
{
    return $GLOBALS['oo-mode'] ? $fmt->setTextAttribute( $attribute, $value ) : numfmt_set_text_attribute( $fmt, $attribute, $value );
}
function ut_nfmt_set_symbol( $fmt, $attribute, $value )
{
    return $GLOBALS['oo-mode'] ? $fmt->setSymbol( $attribute, $value ) : numfmt_set_symbol( $fmt, $attribute, $value );
}
function ut_nfmt_set_pattern( $fmt, $pattern )
{
    return $GLOBALS['oo-mode'] ? $fmt->setPattern( $pattern ) : numfmt_set_pattern( $fmt, $pattern );
}
function ut_nfmt_get_attribute( $fmt, $attribute )
{
    return $GLOBALS['oo-mode'] ? $fmt->getAttribute( $attribute ) : numfmt_get_attribute( $fmt, $attribute );
}
function ut_nfmt_get_text_attribute( $fmt, $attribute )
{
    return $GLOBALS['oo-mode'] ? $fmt->getTextAttribute( $attribute ) : numfmt_get_text_attribute( $fmt, $attribute );
}
function ut_nfmt_get_symbol( $fmt, $attribute )
{
    return $GLOBALS['oo-mode'] ? $fmt->getSymbol( $attribute ) : numfmt_get_symbol( $fmt, $attribute );
}
function ut_nfmt_get_pattern( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getPattern() : numfmt_get_pattern( $fmt );
}
function ut_nfmt_get_locale( $fmt, $type = 0 )
{
    return $GLOBALS['oo-mode'] ? $fmt->getLocale( $type ) : numfmt_get_locale( $fmt, $type );
}
function ut_nfmt_get_error_code( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getErrorCode() : numfmt_get_error_code( $fmt );
}
function ut_nfmt_get_error_message( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getErrorMessage() : numfmt_get_error_message( $fmt );
}

function ut_norm_normalize( $str, $form )
{
    return $GLOBALS['oo-mode'] ? Normalizer::normalize( $str, $form ) : normalizer_normalize( $str, $form );
}
function ut_norm_is_normalized( $str, $form )
{
    return $GLOBALS['oo-mode'] ? Normalizer::isNormalized( $str, $form ) : normalizer_is_normalized( $str, $form );
}

/*
 * Wrappers around Collator methods to run them in either OO- or procedural mode.
 */

function ut_loc_get_default( )
{
    return $GLOBALS['oo-mode'] ? Locale::getDefault( ) : locale_get_default();
}
function ut_loc_set_default( $locale  )
{
   	return $GLOBALS['oo-mode'] ? Locale::setDefault( $locale  ) : locale_set_default( $locale );
}
function ut_loc_get_primary_language( $locale )
{
    return $GLOBALS['oo-mode'] ? Locale::getPrimaryLanguage( $locale ) : locale_get_primary_language( $locale );
}
function ut_loc_get_script( $locale )
{
    return $GLOBALS['oo-mode'] ? Locale::getScript( $locale ) : locale_get_script( $locale );
}
function ut_loc_get_region( $locale )
{
    return $GLOBALS['oo-mode'] ? Locale::getRegion( $locale ) : locale_get_region( $locale );
}
function ut_loc_get_keywords( $locale )
{
    return $GLOBALS['oo-mode'] ? Locale::getKeywords( $locale ) : locale_get_keywords( $locale );
}
function ut_loc_get_display_name( $locale , $dispLocale )
{
    return $GLOBALS['oo-mode'] ? Locale::getDisplayName( $locale , $dispLocale ) : locale_get_display_name( $locale , $dispLocale );
}
function ut_loc_get_display_language( $locale , $dispLocale )
{
    return $GLOBALS['oo-mode'] ? Locale::getDisplayLanguage( $locale , $dispLocale ) : locale_get_display_language( $locale , $dispLocale );
}
function ut_loc_get_display_script( $locale , $dispLocale )
{
    return $GLOBALS['oo-mode'] ? Locale::getDisplayScript( $locale , $dispLocale ) : locale_get_display_script( $locale , $dispLocale );
}
function ut_loc_get_display_region( $locale, $dispLocale  )
{
    return $GLOBALS['oo-mode'] ? Locale::getDisplayRegion( $locale, $dispLocale  ) : locale_get_display_region( $locale, $dispLocale  );
}
function ut_loc_get_display_variant( $locale , $dispLocale )
{
    return $GLOBALS['oo-mode'] ? Locale::getDisplayVariant( $locale , $dispLocale ) : locale_get_display_variant( $locale, $dispLocale  );
}
function ut_loc_locale_compose( $loc_parts_arr )
{
    return $GLOBALS['oo-mode'] ? Locale::composeLocale( $loc_parts_arr ) : locale_compose( $loc_parts_arr );
}
function ut_loc_locale_parse( $locale )
{
    return $GLOBALS['oo-mode'] ? Locale::parseLocale( $locale ) : locale_parse($locale );
}
function ut_loc_locale_get_all_variants( $locale )
{
    return $GLOBALS['oo-mode'] ? Locale::getAllVariants( $locale ) : locale_get_all_variants( $locale );
}
function ut_loc_locale_filter_matches( $lang_tag,$loc_range  ,$isCanonical)
{
    return $GLOBALS['oo-mode'] ? Locale::filterMatches( $lang_tag,$loc_range ,$isCanonical) : locale_filter_matches( $lang_tag,$loc_range ,$isCanonical);
}
function ut_loc_canonicalize( $locale )
{
    return $GLOBALS['oo-mode'] ? Locale::canonicalize( $locale ) : locale_canonicalize( $locale );
}
function ut_loc_locale_lookup( $lang_tag_arr,$loc_range,$isCanonical,$default_loc)
{
    return $GLOBALS['oo-mode'] ? Locale::lookup( $lang_tag_arr,$loc_range,$isCanonical,$default_loc ) : locale_lookup( $lang_tag_arr,$loc_range,$isCanonical,$default_loc );
}
function ut_loc_accept_http($http) {
	return $GLOBALS['oo-mode'] ? Locale::acceptFromHttp($http):locale_accept_from_http($http);
}
/* MessageFormatter functions */
function ut_msgfmt_create( $locale, $pattern)
{
    return $GLOBALS['oo-mode'] ? MessageFormatter::create( $locale, $pattern ) : msgfmt_create( $locale, $pattern );
}
function ut_msgfmt_format( $fmt, $args )
{
    return $GLOBALS['oo-mode'] ? $fmt->format( $args ) : msgfmt_format( $fmt, $args);
}
function ut_msgfmt_parse( $fmt, $string)
{
    return $GLOBALS['oo-mode'] ? $fmt->parse( $string) : msgfmt_parse( $fmt, $string);
}
function ut_msgfmt_format_message( $locale, $pattern, $args )
{
    return $GLOBALS['oo-mode'] ? MessageFormatter::formatMessage( $locale, $pattern, $args ) : msgfmt_format_message( $locale, $pattern, $args );
}
function ut_msgfmt_parse_message( $locale, $pattern, $string )
{
    return $GLOBALS['oo-mode'] ? MessageFormatter::parseMessage( $locale, $pattern, $string ) : msgfmt_parse_message( $locale, $pattern, $string );
}
function ut_msgfmt_set_pattern( $fmt, $pattern )
{
    return $GLOBALS['oo-mode'] ? $fmt->setPattern( $pattern ) : msgfmt_set_pattern( $fmt, $pattern );
}
function ut_msgfmt_get_pattern( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getPattern() : msgfmt_get_pattern( $fmt );
}
function ut_msgfmt_get_locale( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getLocale( ) : msgfmt_get_locale( $fmt );
}
function ut_msgfmt_get_error_code( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getErrorCode() : msgfmt_get_error_code( $fmt );
}
function ut_msgfmt_get_error_message( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getErrorMessage() : msgfmt_get_error_message( $fmt );
}
/* IntlDateFormatter functions */
function ut_datefmt_create( $locale, $datetype, $timetype, $timezone = null, $calendar = null ,$pattern = null)
{
    return $GLOBALS['oo-mode'] ? datefmt_create( $locale, $datetype, $timetype, $timezone, $calendar ,$pattern ) : datefmt_create( $locale, $datetype, $timetype, $timezone, $calendar ,$pattern);
}
function ut_datefmt_get_datetype( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getDateType( ) : datefmt_get_datetype( $fmt );
}
function ut_datefmt_get_timetype( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getTimeType( ) : datefmt_get_timetype( $fmt );
}
function ut_datefmt_get_calendar( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getCalendar( ) : datefmt_get_calendar( $fmt );
}
function ut_datefmt_set_calendar( $fmt ,$calendar )
{
    return $GLOBALS['oo-mode'] ? $fmt->setCalendar( $calendar ) : datefmt_set_calendar( $fmt , $calendar );
}
function ut_datefmt_get_timezone_id( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getTimeZoneId( ) : datefmt_get_timezone_id( $fmt );
}
function ut_datefmt_set_timezone_id( $fmt ,$timezone_id )
{
    return $GLOBALS['oo-mode'] ? $fmt->setTimeZoneId( $timezone_id ) : datefmt_set_timezone_id( $fmt ,$timezone_id);
}
function ut_datefmt_get_pattern( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->getPattern() : datefmt_get_pattern( $fmt );
}
function ut_datefmt_set_pattern( $fmt , $pattern )
{
    return $GLOBALS['oo-mode'] ? $fmt->setPattern( $pattern ) : datefmt_set_pattern( $fmt ,  $pattern);
}
function ut_datefmt_get_locale( $fmt ,$type=ULOC_ACTUAL_LOCALE)
{
    return $GLOBALS['oo-mode'] ? $fmt->getLocale($type ) : datefmt_get_locale( $fmt ,$type);
}
function ut_datefmt_is_lenient( $fmt )
{
    return $GLOBALS['oo-mode'] ? $fmt->isLenient() : datefmt_is_lenient( $fmt );
}
function ut_datefmt_set_lenient( $fmt , $lenient )
{
    return $GLOBALS['oo-mode'] ? $fmt->setLenient(  $lenient ) : datefmt_set_lenient( $fmt ,  $lenient);
}
function ut_datefmt_format( $fmt , $value )
{
    return $GLOBALS['oo-mode'] ? $fmt->format(  $value ) : datefmt_format( $fmt ,  $value);
}
function ut_datefmt_parse( $fmt , $value , &$parse_pos=0 )
{
    return $GLOBALS['oo-mode'] ? $fmt->parse(  $value ,$parse_pos ) : datefmt_parse( $fmt ,  $value,$parse_pos);
}
function ut_datefmt_localtime( $fmt , $value , &$parse_pos=0 )
{
    return $GLOBALS['oo-mode'] ? $fmt->localtime(  $value , $parse_pos ) : datefmt_localtime( $fmt ,  $value , $parse_pos );
}

function ut_resourcebundle_create( $locale, $bundle, $fallback=true )
{
    return $GLOBALS['oo-mode'] ? new ResourceBundle($locale, $bundle, $fallback): resourcebundle_create($locale, $bundle, $fallback);
}
function ut_resourcebundle_count($bundle )
{
    return $GLOBALS['oo-mode'] ? $bundle->count():resourcebundle_count($bundle);
}
function ut_resourcebundle_locales($bundle )
{
    return $GLOBALS['oo-mode'] ? ResourceBundle::getLocales($bundle):resourcebundle_locales($bundle);
}
function ut_resourcebundle_get($bundle, $idx )
{
    return $GLOBALS['oo-mode'] ? $bundle->get($idx):resourcebundle_get($bundle, $idx);
}
function ut_resourcebundle_get_error_code($bundle )
{
    return $GLOBALS['oo-mode'] ? $bundle->getErrorCode():resourcebundle_get_error_code($bundle);
}
function ut_resourcebundle_get_error_message($bundle )
{
    return $GLOBALS['oo-mode'] ? $bundle->getErrorMessage():resourcebundle_get_error_message($bundle);
}
