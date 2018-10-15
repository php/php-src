--TEST--
IntlTimeZone::createTimeZoneIDEnumeration(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createTimeZoneIDEnumeration());
var_dump(IntlTimeZone::createTimeZoneIDEnumeration(array()));
var_dump(IntlTimeZone::createTimeZoneIDEnumeration(-1));
var_dump(IntlTimeZone::createTimeZoneIDEnumeration(IntlTimeZone::TYPE_ANY, array()));
var_dump(IntlTimeZone::createTimeZoneIDEnumeration(IntlTimeZone::TYPE_ANY, "PT", "a80"));
--EXPECTF--
Warning: IntlTimeZone::createTimeZoneIDEnumeration() expects at least 1 parameter, 0 given in %s on line %d

Warning: IntlTimeZone::createTimeZoneIDEnumeration(): intltz_create_time_zone_id_enumeration: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::createTimeZoneIDEnumeration() expects parameter 1 to be integer, array given in %s on line %d

Warning: IntlTimeZone::createTimeZoneIDEnumeration(): intltz_create_time_zone_id_enumeration: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::createTimeZoneIDEnumeration(): intltz_create_time_zone_id_enumeration: bad zone type in %s on line %d
bool(false)

Warning: IntlTimeZone::createTimeZoneIDEnumeration() expects parameter 2 to be string, array given in %s on line %d

Warning: IntlTimeZone::createTimeZoneIDEnumeration(): intltz_create_time_zone_id_enumeration: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::createTimeZoneIDEnumeration() expects parameter 3 to be integer, string given in %s on line %d

Warning: IntlTimeZone::createTimeZoneIDEnumeration(): intltz_create_time_zone_id_enumeration: bad arguments in %s on line %d
bool(false)
