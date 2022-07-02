--TEST--
Bug #70454 (IntlChar::forDigit second parameter should be optional)
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(IntlChar::forDigit(0));
var_dump(IntlChar::forDigit(3));
var_dump(IntlChar::forDigit(3, 10));
var_dump(IntlChar::forDigit(10));
var_dump(IntlChar::forDigit(10, 16));
?>
--EXPECT--
int(48)
int(51)
int(51)
int(0)
int(97)
