--TEST--
get_locale() icu <= 4.2
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.3', '<') != 1) print 'skip'; ?>
--FILE--
<?php

/*
 * Try to specify valid and invalid locale types when getting locale.
 */

function ut_main()
{
    $locales = array(
        Locale::VALID_LOCALE,
        Locale::ACTUAL_LOCALE,
        100,
        -100,
        -9999999999999,
        9999999999999,
        1.2,
    );

    $coll = ut_coll_create( 'en_US' );
    $res_str = '';

    foreach( $locales as $locale )
    {
        $rc = ut_coll_get_locale( $coll, $locale );

        $res_str .= sprintf(
            "Locale of type %s is %s\n",
            dump( $locale ),
            dump( $rc ) );
    }

    return $res_str . "\n";
}

include_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
Locale of type 1 is 'en_US'
Locale of type 0 is 'en'
Locale of type 100 is false
Locale of type -100 is false
Locale of type -9999999999999 is false
Locale of type 9999999999999 is false
Locale of type 1.2 is 'en_US'
