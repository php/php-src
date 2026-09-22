--TEST--
IntlListFormatter getErrorCode()/getErrorMessage() reflect format() failures
--EXTENSIONS--
intl
--FILE--
<?php

$formatter = new IntlListFormatter('en_US');

var_dump($formatter->format(["\x80"]));
var_dump($formatter->getErrorCode() === U_INVALID_CHAR_FOUND);
var_dump($formatter->getErrorMessage());

var_dump($formatter->format(['a', 'b']));
var_dump($formatter->getErrorCode() === U_ZERO_ERROR);
var_dump($formatter->getErrorMessage());

?>
--EXPECT--
bool(false)
bool(true)
string(85) "IntlListFormatter::format(): Failed to convert string to UTF-16: U_INVALID_CHAR_FOUND"
string(7) "a and b"
bool(true)
string(12) "U_ZERO_ERROR"
