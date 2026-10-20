--TEST--
IntlNumberFormatter::format() preserves int64 precision
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists('IntlNumberFormatter')) {
    die('skip IntlNumberFormatter not available');
}
if (PHP_INT_SIZE < 8) {
    die('skip 64-bit only');
}
?>
--FILE--
<?php

$formatter = IntlNumberFormatter::createFromSkeleton('', 'en_US');

var_dump($formatter->format(9007199254740993));
var_dump($formatter->format(PHP_INT_MIN));

?>
--EXPECT--
string(21) "9,007,199,254,740,993"
string(26) "-9,223,372,036,854,775,808"
