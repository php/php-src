--TEST--
IntlTimeZone::getEquivalentID(): errors
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::getEquivalentID("foo\x80", 0));
echo intl_get_error_message(), PHP_EOL;

?>
--EXPECT--
bool(false)
IntlTimeZone::getEquivalentID(): could not convert time zone id to UTF-16: U_INVALID_CHAR_FOUND
