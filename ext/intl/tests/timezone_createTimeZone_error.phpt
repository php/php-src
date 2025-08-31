--TEST--
IntlTimeZone::createTimeZone(): errors
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::createTimeZone("foo\x80"));
echo intl_get_error_message(), PHP_EOL;

?>
--EXPECT--
NULL
IntlTimeZone::createTimeZone(): could not convert time zone id to UTF-16: U_INVALID_CHAR_FOUND
