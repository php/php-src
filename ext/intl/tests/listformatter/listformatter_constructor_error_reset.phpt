--TEST--
IntlListFormatter::__construct() resets stale errors
--EXTENSIONS--
intl
--FILE--
<?php
$formatter = new IntlListFormatter('en_US');
var_dump($formatter->format(["\x80"]));
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);

$formatter = new IntlListFormatter('en_US');
var_dump(intl_get_error_code());
var_dump(intl_get_error_message());
var_dump($formatter->getErrorCode());
var_dump($formatter->getErrorMessage());
?>
--EXPECT--
bool(false)
bool(true)
int(0)
string(12) "U_ZERO_ERROR"
int(0)
string(12) "U_ZERO_ERROR"
