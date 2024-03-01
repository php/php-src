--TEST--
IntlTimeZone::countEquivalentIDs(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::countEquivalentIDs("foo\x80"));
?>
--EXPECTF--
Warning: IntlTimeZone::countEquivalentIDs(): could not convert time zone id to UTF-16 in %s on line %d
bool(false)
