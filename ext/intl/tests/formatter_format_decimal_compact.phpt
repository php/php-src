--TEST--
numfmt_format_currency() icu >= 4.8
--EXTENSIONS--
intl
--FILE--
<?php

function ut_main()
{
    $locales = array(
        'en_UK',
        'en_US',
        'ru',
        'zh_CN',
        'ro',
        'uk',
        'en',
        'bg'
    );

    $number = 1234567.89;

    $res_str = '';
    foreach( $locales as $locale )
    {
        $fmt = ut_nfmt_create( $locale, NumberFormatter::DECIMAL_COMPACT_SHORT );
        $res_str .= "$locale: short = " . ut_nfmt_format ($fmt, $number) . "\n";

        $fmt = ut_nfmt_create( $locale, NumberFormatter::DECIMAL_COMPACT_LONG );
        $res_str .= "$locale: long = " . ut_nfmt_format ($fmt, $number) . "\n";
    }
    return $res_str;
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();

?>
--EXPECTF--
en_UK: short = 1.2M
en_UK: long = 1.2 million
en_US: short = 1.2M
en_US: long = 1.2 million
ru: short = 1,2 млн
ru: long = 1,2 миллиона
zh_CN: short = 123万
zh_CN: long = 123万
ro: short = 1,2 mil.
ro: long = 1,2 milioane
uk: short = 1,2 млн
uk: long = 1,2 мільйона
en: short = 1.2M
en: long = 1.2 million
bg: short = 1,2 млн.
bg: long = 1,2 милиона