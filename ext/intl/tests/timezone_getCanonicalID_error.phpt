--TEST--
IntlTimeZone::getCanonicalID(): errors
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::getCanonicalID("foo\x81"));
echo intl_get_error_message(), PHP_EOL;

?>
--EXPECT--
bool(false)
IntlTimeZone::getCanonicalID(): could not convert time zone id to UTF-16: U_INVALID_CHAR_FOUND
