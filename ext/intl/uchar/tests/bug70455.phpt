--TEST--
Bug #70455 Missing constant: IntlChar::NO_NUMERIC_VALUE
--SKIPIF--
<?php if (!extension_loaded('intl')) die("skip requires ext/intl") ?>
--FILE--
<?php
$value = IntlChar::getNumericValue("x");
var_dump($value);
var_dump($value === IntlChar::NO_NUMERIC_VALUE);
?>
--EXPECT--
float(-123456789)
bool(true)
