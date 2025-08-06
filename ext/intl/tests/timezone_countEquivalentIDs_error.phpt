--TEST--
IntlTimeZone::countEquivalentIDs(): errors
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::countEquivalentIDs("foo\x80"));
echo intl_get_error_message(), PHP_EOL;

?>
--EXPECT--
bool(false)
IntlTimeZone::countEquivalentIDs(): could not convert time zone id to UTF-16: U_INVALID_CHAR_FOUND
