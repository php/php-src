--TEST--
Collator creation tests
--EXTENSIONS--
intl
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
        '',
        'root',
        'uk@currency=EURO',
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
  ULOC_VALID_LOCALE     = 'uk'
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
