--TEST--
Bug #70455 Missing constant: IntlChar::NO_NUMERIC_VALUE
--EXTENSIONS--
intl
--FILE--
<?php
$value = IntlChar::getNumericValue("x");
var_dump($value);
var_dump($value === IntlChar::NO_NUMERIC_VALUE);
?>
--EXPECT--
float(-123456789)
bool(true)
