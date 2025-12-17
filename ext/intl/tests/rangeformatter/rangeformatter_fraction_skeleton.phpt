--TEST--
Test precision skeleton with IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY
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

$precisions = ['.##', '.#'];

foreach ($precisions as $precision) {
    echo 'Approximate with ' . $precision . PHP_EOL;

    $nrf = IntlNumberRangeFormatter::createFromSkeleton(
        $precision, 
        'en_GB', 
        IntlNumberRangeFormatter::COLLAPSE_AUTO, 
        IntlNumberRangeFormatter::IDENTITY_FALLBACK_APPROXIMATELY
    );
    var_dump($nrf->format(5.1, 5.2));
    var_dump($nrf->format(5.01, 5.02));
    var_dump($nrf->format(5.001, 5.002));
}

?>
--EXPECT--
Approximate with .##
string(9) "5.1–5.2"
string(11) "5.01–5.02"
string(2) "~5"
Approximate with .#
string(9) "5.1–5.2"
string(2) "~5"
string(2) "~5"
