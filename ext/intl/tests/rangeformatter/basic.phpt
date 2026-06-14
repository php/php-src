--TEST--
Basic test for IntlNumberRangeFormatter
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '63.0') < 0) {
    die('skip for ICU < 63.0');
}
?>
--FILE--
<?php

$languages = ['en_US', 'RO', 'JA'];

$identityFallbacks = [
    'IDENTITY_FALLBACK_SINGLE_VALUE' => IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE, 
    'IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE' => IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE,
    'IDENTITY_FALLBACK_APPROXIMATELY' => IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY,
    'IDENTITY_FALLBACK_RANGE' => IntlNumberRangeFormatter::IDENTITY_FALLBACK_RANGE
];

$collapses = [
    'COLLAPSE_AUTO' => IntlNumberRangeFormatter::COLLAPSE_AUTO,
    'COLLAPSE_NONE' => IntlNumberRangeFormatter::COLLAPSE_NONE,
    'COLLAPSE_UNIT' => IntlNumberRangeFormatter::COLLAPSE_UNIT,
    'COLLAPSE_ALL' => IntlNumberRangeFormatter::COLLAPSE_ALL
];

foreach ($languages as $language) {
    foreach ($identityFallbacks as $iName => $identityFallback) {
        foreach ($collapses as $cName => $collapse) {
            echo PHP_EOL . $language . ' - ' . $cName . ' - ' . $iName . PHP_EOL;
            $nrf = IntlNumberRangeFormatter::createFromSkeleton(
                'measure-unit/length-meter', 
                $language, 
                $collapse, 
                $identityFallback
            );

            var_dump($nrf->format(1.1, 2.2));
            var_dump($nrf->format(100, 200));
            var_dump($nrf->format(-5, 5));
            var_dump($nrf->format(5, 5));
            var_dump($nrf->format(5.0001, 5.0001));
            var_dump($nrf->format(5, 5.1));
            var_dump($nrf->format(5.1, 6));
        }
    }
}

?>
--EXPECT--
en_US - COLLAPSE_AUTO - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_NONE - IDENTITY_FALLBACK_SINGLE_VALUE
string(15) "1.1 m – 2.2 m"
string(15) "100 m – 200 m"
string(12) "-5 m – 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(13) "5 m – 5.1 m"
string(13) "5.1 m – 6 m"

en_US - COLLAPSE_UNIT - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_ALL - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_AUTO - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_NONE - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(15) "1.1 m – 2.2 m"
string(15) "100 m – 200 m"
string(12) "-5 m – 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(13) "5 m – 5.1 m"
string(13) "5.1 m – 6 m"

en_US - COLLAPSE_UNIT - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_ALL - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_AUTO - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(4) "~5 m"
string(9) "~5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_NONE - IDENTITY_FALLBACK_APPROXIMATELY
string(15) "1.1 m – 2.2 m"
string(15) "100 m – 200 m"
string(12) "-5 m – 5 m"
string(4) "~5 m"
string(9) "~5.0001 m"
string(13) "5 m – 5.1 m"
string(13) "5.1 m – 6 m"

en_US - COLLAPSE_UNIT - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(4) "~5 m"
string(9) "~5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_ALL - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(4) "~5 m"
string(9) "~5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_AUTO - IDENTITY_FALLBACK_RANGE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(7) "5–5 m"
string(17) "5.0001–5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_NONE - IDENTITY_FALLBACK_RANGE
string(15) "1.1 m – 2.2 m"
string(15) "100 m – 200 m"
string(12) "-5 m – 5 m"
string(11) "5 m – 5 m"
string(21) "5.0001 m – 5.0001 m"
string(13) "5 m – 5.1 m"
string(13) "5.1 m – 6 m"

en_US - COLLAPSE_UNIT - IDENTITY_FALLBACK_RANGE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(7) "5–5 m"
string(17) "5.0001–5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

en_US - COLLAPSE_ALL - IDENTITY_FALLBACK_RANGE
string(11) "1.1–2.2 m"
string(11) "100–200 m"
string(10) "-5 – 5 m"
string(7) "5–5 m"
string(17) "5.0001–5.0001 m"
string(9) "5–5.1 m"
string(9) "5.1–6 m"

RO - COLLAPSE_AUTO - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(3) "5 m"
string(8) "5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_NONE - IDENTITY_FALLBACK_SINGLE_VALUE
string(13) "1,1 m - 2,2 m"
string(13) "100 m - 200 m"
string(10) "-5 m - 5 m"
string(3) "5 m"
string(8) "5,0001 m"
string(11) "5 m - 5,1 m"
string(11) "5,1 m - 6 m"

RO - COLLAPSE_UNIT - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(3) "5 m"
string(8) "5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_ALL - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(3) "5 m"
string(8) "5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_AUTO - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(3) "5 m"
string(8) "5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_NONE - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(13) "1,1 m - 2,2 m"
string(13) "100 m - 200 m"
string(10) "-5 m - 5 m"
string(3) "5 m"
string(8) "5,0001 m"
string(11) "5 m - 5,1 m"
string(11) "5,1 m - 6 m"

RO - COLLAPSE_UNIT - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(3) "5 m"
string(8) "5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_ALL - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(3) "5 m"
string(8) "5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_AUTO - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(4) "~5 m"
string(9) "~5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_NONE - IDENTITY_FALLBACK_APPROXIMATELY
string(13) "1,1 m - 2,2 m"
string(13) "100 m - 200 m"
string(10) "-5 m - 5 m"
string(4) "~5 m"
string(9) "~5,0001 m"
string(11) "5 m - 5,1 m"
string(11) "5,1 m - 6 m"

RO - COLLAPSE_UNIT - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(4) "~5 m"
string(9) "~5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_ALL - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(4) "~5 m"
string(9) "~5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_AUTO - IDENTITY_FALLBACK_RANGE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(7) "5 - 5 m"
string(17) "5,0001 - 5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_NONE - IDENTITY_FALLBACK_RANGE
string(13) "1,1 m - 2,2 m"
string(13) "100 m - 200 m"
string(10) "-5 m - 5 m"
string(9) "5 m - 5 m"
string(19) "5,0001 m - 5,0001 m"
string(11) "5 m - 5,1 m"
string(11) "5,1 m - 6 m"

RO - COLLAPSE_UNIT - IDENTITY_FALLBACK_RANGE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(7) "5 - 5 m"
string(17) "5,0001 - 5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

RO - COLLAPSE_ALL - IDENTITY_FALLBACK_RANGE
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
string(7) "5 - 5 m"
string(17) "5,0001 - 5,0001 m"
string(9) "5 - 5,1 m"
string(9) "5,1 - 6 m"

JA - COLLAPSE_AUTO - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_NONE - IDENTITY_FALLBACK_SINGLE_VALUE
string(15) "1.1 m ～ 2.2 m"
string(15) "100 m ～ 200 m"
string(12) "-5 m ～ 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(13) "5 m ～ 5.1 m"
string(13) "5.1 m ～ 6 m"

JA - COLLAPSE_UNIT - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_ALL - IDENTITY_FALLBACK_SINGLE_VALUE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_AUTO - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_NONE - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(15) "1.1 m ～ 2.2 m"
string(15) "100 m ～ 200 m"
string(12) "-5 m ～ 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(13) "5 m ～ 5.1 m"
string(13) "5.1 m ～ 6 m"

JA - COLLAPSE_UNIT - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_ALL - IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(3) "5 m"
string(8) "5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_AUTO - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(6) "約5 m"
string(11) "約5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_NONE - IDENTITY_FALLBACK_APPROXIMATELY
string(15) "1.1 m ～ 2.2 m"
string(15) "100 m ～ 200 m"
string(12) "-5 m ～ 5 m"
string(6) "約5 m"
string(11) "約5.0001 m"
string(13) "5 m ～ 5.1 m"
string(13) "5.1 m ～ 6 m"

JA - COLLAPSE_UNIT - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(6) "約5 m"
string(11) "約5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_ALL - IDENTITY_FALLBACK_APPROXIMATELY
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(6) "約5 m"
string(11) "約5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_AUTO - IDENTITY_FALLBACK_RANGE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(7) "5～5 m"
string(17) "5.0001～5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_NONE - IDENTITY_FALLBACK_RANGE
string(15) "1.1 m ～ 2.2 m"
string(15) "100 m ～ 200 m"
string(12) "-5 m ～ 5 m"
string(11) "5 m ～ 5 m"
string(21) "5.0001 m ～ 5.0001 m"
string(13) "5 m ～ 5.1 m"
string(13) "5.1 m ～ 6 m"

JA - COLLAPSE_UNIT - IDENTITY_FALLBACK_RANGE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(7) "5～5 m"
string(17) "5.0001～5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"

JA - COLLAPSE_ALL - IDENTITY_FALLBACK_RANGE
string(11) "1.1～2.2 m"
string(11) "100～200 m"
string(10) "-5 ～ 5 m"
string(7) "5～5 m"
string(17) "5.0001～5.0001 m"
string(9) "5～5.1 m"
string(9) "5.1～6 m"
