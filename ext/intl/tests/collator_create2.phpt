--TEST--
create() icu >= 4.8 && icu < 53.1
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.8') < 0) print 'skip for ICU >= 4.8'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '53.1') >=  0) die('skip for ICU < 53.1'); ?>
--FILE--
<?php

/*
 * Try creating collator with different locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $res_str = '';

    $locales = array(
        'EN-US-ODESSA',
        'UK_UA_ODESSA',
        'uk-ua_CALIFORNIA@currency=;currency=GRN',
        '',
        'root',
        'uk@currency=EURO',
	'1234567891113151719212325272931333537394143454749515357596163656769717375777981838587899193959799'
    );

    foreach( $locales as $locale )
    {
        // Create Collator with the current locale.
        $coll = ut_coll_create( $locale );
        if( !is_object($coll) )
        {
            $res_str .= "Error creating collator with '$locale' locale: " .
                 intl_get_error_message() . "\n";
            continue;
        }

        // Get the requested, valid and actual locales.
        $vloc = ut_coll_get_locale( $coll, Locale::VALID_LOCALE );
        $aloc = ut_coll_get_locale( $coll, Locale::ACTUAL_LOCALE );

        // Show them.
        $res_str .= "Locale: '$locale'\n" .
            "  ULOC_REQUESTED_LOCALE = '$locale'\n" .
            "  ULOC_VALID_LOCALE     = '$vloc'\n" .
            "  ULOC_ACTUAL_LOCALE    = '$aloc'\n";
    }

    return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECTF--
Locale: 'EN-US-ODESSA'
  ULOC_REQUESTED_LOCALE = 'EN-US-ODESSA'
  ULOC_VALID_LOCALE     = 'en_US'
  ULOC_ACTUAL_LOCALE    = 'root'
Locale: 'UK_UA_ODESSA'
  ULOC_REQUESTED_LOCALE = 'UK_UA_ODESSA'
  ULOC_VALID_LOCALE     = 'uk_UA'
  ULOC_ACTUAL_LOCALE    = 'uk'
Locale: 'uk-ua_CALIFORNIA@currency=;currency=GRN'
  ULOC_REQUESTED_LOCALE = 'uk-ua_CALIFORNIA@currency=;currency=GRN'
  ULOC_VALID_LOCALE     = 'uk_UA'
  ULOC_ACTUAL_LOCALE    = 'uk'
Locale: ''
  ULOC_REQUESTED_LOCALE = ''
  ULOC_VALID_LOCALE     = '%s'
  ULOC_ACTUAL_LOCALE    = '%s'
Locale: 'root'
  ULOC_REQUESTED_LOCALE = 'root'
  ULOC_VALID_LOCALE     = 'root'
  ULOC_ACTUAL_LOCALE    = 'root'
Locale: 'uk@currency=EURO'
  ULOC_REQUESTED_LOCALE = 'uk@currency=EURO'
  ULOC_VALID_LOCALE     = 'uk'
  ULOC_ACTUAL_LOCALE    = 'uk'
Error creating collator with '1234567891113151719212325272931333537394143454749515357596163656769717375777981838587899193959799' locale: Locale string too long, should be no longer than 80 characters: U_ILLEGAL_ARGUMENT_ERROR
