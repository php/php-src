--TEST--
IntlTimeZone::getEquivalentID(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getEquivalentID("foo\x80", 0));
?>
--EXPECTF--
Warning: IntlTimeZone::getEquivalentID(): could not convert time zone id to UTF-16 in %s on line %d
bool(false)
