--TEST--
IntlTimeZone::getRegion(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getRegion("foo\x81"));
var_dump(IntlTimeZone::getRegion("foo"));
?>
--EXPECTF--
Warning: IntlTimeZone::getRegion(): could not convert time zone id to UTF-16 in %s on line %d
bool(false)

Warning: IntlTimeZone::getRegion(): error obtaining region in %s on line %d
bool(false)
