--TEST--
numfmt_format_currency()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Format a number using misc currencies/locales.
 */
/* 
 * TODO: doesn't pass on ICU 3.6 because 'ru' and 'uk' locales changed
 * currency formatting.
 */


function ut_main()
{
    $locales = array(
        'en_UK' => 'GBP',
        'en_US' => 'USD',
        'ru'    => 'RUR',
        'uk'    => 'UAH',
        'en'    => 'UAH'
    );

    $res_str = '';
    $number = 1234567.89;

    foreach( $locales as $locale => $currency )
    {
        $fmt = ut_nfmt_create( $locale, NumberFormatter::CURRENCY );
        $res_str .= "$locale: " . var_export( ut_nfmt_format_currency( $fmt, $number, $currency ), true ) . "\n";
    }
    return $res_str;
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();

?>
--EXPECTREGEX--
en_UK: '£1,234,567.89'
en_US: '\$1,234,567.89'
ru: '1 234 567,89 ?р.'
uk: '(грн. )?1 234 567,89( грн.)?'
en: 'UAH1,234,567.89'