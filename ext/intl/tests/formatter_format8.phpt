--TEST--
numfmt_format() icu >= 62.1
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '62.1') < 0) die('skip for ICU >= 62.1'); ?>
--FILE--
<?php

/*
 * Format a number using misc locales/patterns.
 */

/*
 * TODO: doesn't pass on ICU 3.6 because 'ru' and 'de' locales changed
 * currency and percent formatting.
 */

function ut_main()
{
    $styles = array(
        NumberFormatter::PATTERN_DECIMAL => '##.#####################',
        NumberFormatter::DECIMAL => '',
        NumberFormatter::CURRENCY => '',
        NumberFormatter::PERCENT => '',
        NumberFormatter::SCIENTIFIC => '',
        NumberFormatter::SPELLOUT => '@@@@@@@',
        NumberFormatter::ORDINAL => '',
        NumberFormatter::DURATION => '',
        NumberFormatter::PATTERN_RULEBASED => '#####.###',
        1234999, // bad one
    );

   $integer = array(
        NumberFormatter::ORDINAL => '',
        NumberFormatter::DURATION => '',
   );
    $locales = array(
        'en_US',
        'ru_UA',
        'de',
        'fr',
        'en_UK'
    );

    $str_res = '';
    $number = 1234567.891234567890000;

    foreach( $locales as $locale )
    {
        $str_res .= "\nLocale is: $locale\n";
        foreach( $styles as $style => $pattern )
        {
            $fmt = ut_nfmt_create( $locale, $style, $pattern );

            if(!$fmt) {
                $str_res .= "Bad formatter!\n";
                continue;
            }
            $str_res .= dump( isset($integer[$style])?ut_nfmt_format( $fmt, $number, NumberFormatter::TYPE_INT32):ut_nfmt_format( $fmt, $number ) ) . "\n";
        }
    }
    return $str_res;
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();

?>
--EXPECTREGEX--
Locale is: en_US
'1234567.8912345\d+'
'1,234,567.891'
'\$1,234,567.89'
'123,456,789%'
'1.2345678912345\d+E6'
'one million,? two hundred (and )?thirty-four thousand,? five hundred (and )?sixty-seven point eight nine one two three four five( six)? seven( nine)?'
'1,234,567(th|ᵗʰ)'
'342:56:07'
'#####.###'
'USD 1,234,567.89'

Locale is: ru_UA
'1234567.8912345\d+'
'1 234 567,891'
'1 234 567,89 ?(грн\.|₴)'
'123 456 789 ?%'
'1.2345678912345\d+E6'
'один миллион двести тридцать четыре тысячи пятьсот шестьдесят семь целых восемьдесят девять миллионов сто двадцать три тысячи четыреста пятьдесят семь стомиллионных'
'1 234 567.?'
'1 234 567'
'#####.###'
'1 234 567,89 UAH'

Locale is: de
'1234567.8912345\d+'
'1.234.567,891'
'1.234.567,89 XXX'
'123\.456\.789 %'
'1.2345678912345\d+E6'
'eine Million zwei­hundert­vier­und­dreißig­tausend­fünf­hundert­sieben­und­sechzig Komma acht neun eins zwei drei vier fünf( sechs)? sieben( neun)?'
'1.234.567.?'
'1.234.567'
'#####.###'
'1.234.567,89 XXX'

Locale is: fr
'1234567.8912345\d+'
'1.+234.+567,891'
'1.+234.+567,89.+(XXX|¤)'
'123.+456.+789(.+)?%'
'1.2345678912345\d+E6'
'un million deux cent trente-quatre mille cinq cent soixante-sept virgule huit neuf un deux trois quatre cinq( six)? sept( neuf)?'
'1.+234.+567e'
'1.+234.+567'
'#####.###'
'1.+234.+567,89.+XXX'

Locale is: en_UK
'1234567.8912345\d+'
'1,234,567.891'
'(¤|XXX )1,234,567.89'
'123,456,789%'
'1.2345678912345\d+E6'
'one million,? two hundred (and )?thirty-four thousand,? five hundred (and )?sixty-seven point eight nine one two three four five( six)? seven( nine)?'
'1,234,567(th|ᵗʰ)'
'342:56:07'
'#####.###'
'XXX 1,234,567.89'
