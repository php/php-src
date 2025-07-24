--TEST--
Basic test for IntlNumberRangeFormatter
--EXTENSIONS--
intl
--FILE--
<?php
$nrf = IntlNumberRangeFormatter::createFromSkeleton(
    'measure-unit/length-meter', 'ro', 
    IntlNumberRangeFormatter::COLLAPSE_AUTO, 
    IntlNumberRangeFormatter::IDENTITY_FALLBACK_SINGLE_VALUE
);

var_dump($nrf->format(1.1, 2.2));
var_dump($nrf->format(100, 200));
var_dump($nrf->format(-5, 5));
?>
--EXPECT--
string(11) "1,1 - 2,2 m"
string(11) "100 - 200 m"
string(8) "-5 - 5 m"
