--TEST--
msgfmt_get_locale()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
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
        'uk',
    );

    $res_str = '';

    foreach( $locales as $locale )
    {
        $fmt = ut_msgfmt_create( $locale, "Test" );
        $res_str .= "$locale: " . dump( ut_msgfmt_get_locale( $fmt ) );
        $res_str .= "\n";
    }

    return $res_str;
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
en_UK: 'en_UK'
en_US@California: 'en_US@California'
uk: 'uk'
