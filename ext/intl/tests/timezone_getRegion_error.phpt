--TEST--
IntlTimeZone::getRegion(): errors
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::getRegion("foo\x81"));
echo intl_get_error_message(), PHP_EOL;
var_dump(IntlTimeZone::getRegion("foo"));
echo intl_get_error_message(), PHP_EOL;

?>
--EXPECT--
bool(false)
IntlTimeZone::getRegion(): could not convert time zone id to UTF-16: U_INVALID_CHAR_FOUND
bool(false)
IntlTimeZone::getRegion(): error obtaining region: U_ILLEGAL_ARGUMENT_ERROR
