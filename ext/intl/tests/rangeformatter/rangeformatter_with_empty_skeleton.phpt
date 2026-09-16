--TEST--
Test an empty skeleton with IntlNumberRangeFormatter
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

$nrf = IntlNumberRangeFormatter::createFromSkeleton(
    '', 
    'en_GB', 
    IntlNumberRangeFormatter::COLLAPSE_AUTO, 
    IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY
);
var_dump($nrf->format(5.1, 5.2));

?>
--EXPECT--
string(9) "5.1–5.2"
