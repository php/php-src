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
Warning: IntlTimeZone::createTimeZoneIDEnumeration(): intltz_create_time_zone_id_enumeration: bad zone type in %s on line %d
bool(false)
