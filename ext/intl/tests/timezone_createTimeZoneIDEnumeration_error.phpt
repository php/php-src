--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createTimeZoneIDEnumeration(-1));
?>
--EXPECTF--
Warning: IntlTimeZone::createTimeZoneIDEnumeration(): bad zone type in %s on line %d
bool(false)
