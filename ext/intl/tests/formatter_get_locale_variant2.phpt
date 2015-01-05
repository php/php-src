--TEST--
numfmt_get_locale()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '51.2') < 0) die('skip for ICU >= 51.2'); ?>
--FILE--
<?php

/*
 * Get locale.
 */

function ut_main()
{
    $locales = array(
        'en_UK',
        'en_US@California',
        'fr_CA',
    );

    $loc_types = array(
        Locale::ACTUAL_LOCALE    => 'actual',
        Locale::VALID_LOCALE     => 'valid',
    );

    $res_str = '';

    foreach( $locales as $locale )
    {
        $fmt = ut_nfmt_create( $locale, NumberFormatter::DECIMAL );
        $res_str .= "$locale: ";
        foreach( $loc_types as $loc_type => $loc_type_name )
            $res_str .= sprintf( " %s=%s",
            $loc_type_name,
            dump( ut_nfmt_get_locale( $fmt, $loc_type ) ) );
        $res_str .= "\n";
    }

    return $res_str;
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
en_UK:  actual='en' valid='en'
en_US@California:  actual='en' valid='en'
fr_CA:  actual='fr' valid='fr_CA'
