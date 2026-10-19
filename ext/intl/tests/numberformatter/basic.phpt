--TEST--
Basic test for IntlNumberFormatter
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists('IntlNumberFormatter')) {
    die('skip IntlNumberFormatter not available');
}
?>
--FILE--
<?php

$cases = [
    ['measure-unit/speed-kilometer-per-hour', 'en_US', 5],
    ['measure-unit/speed-kilometer-per-hour unit-width-full-name', 'en_US', 5],
    ['measure-unit/speed-kilometer-per-hour unit-width-full-name', 'en_US', 1],
    ['measure-unit/speed-kilometer-per-hour unit-width-full-name', 'en_US', 1.5],
    ['measure-unit/speed-kilometer-per-hour unit-width-full-name', 'de', 1.5],
    ['measure-unit/length-meter per-measure-unit/duration-second', 'en_US', 5],
    ['measure-unit/digital-gigabyte unit-width-narrow', 'en_US', 1.5],
    ['currency/EUR unit-width-full-name', 'en_US', 1234.5],
    ['percent', 'en_US', 25],
    ['compact-short', 'en_US', 1234567],
    ['.00', 'en_US', 1234.5],
    ['group-off', 'en_US', 1234567],
    ['sign-always', 'en_US', 5],
    ['', 'en_US', 1234.5678],
    ['', 'en_US', NAN],
];

foreach ($cases as [$skeleton, $locale, $value]) {
    $formatter = IntlNumberFormatter::createFromSkeleton($skeleton, $locale);
    var_dump($formatter->format($value));
}

var_dump($formatter->getErrorCode());
var_dump($formatter->getErrorMessage());

?>
--EXPECT--
string(6) "5 km/h"
string(21) "5 kilometers per hour"
string(20) "1 kilometer per hour"
string(23) "1.5 kilometers per hour"
string(24) "1,5 Kilometer pro Stunde"
string(5) "5 m/s"
string(5) "1.5GB"
string(14) "1,234.50 euros"
string(3) "25%"
string(4) "1.2M"
string(8) "1,234.50"
string(7) "1234567"
string(2) "+5"
string(10) "1,234.5678"
string(3) "NaN"
int(0)
string(12) "U_ZERO_ERROR"
