--TEST--
IntlTimeZone::getRegion(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getRegion());
var_dump(IntlTimeZone::getRegion(array()));
var_dump(IntlTimeZone::getRegion('Europe/Lisbon', 4));
var_dump(IntlTimeZone::getRegion("foo\x81"));
var_dump(IntlTimeZone::getRegion("foo"));
--EXPECTF--
Warning: IntlTimeZone::getRegion() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlTimeZone::getRegion(): intltz_get_region: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getRegion() expects parameter 1 to be string, array given in %s on line %d

Warning: IntlTimeZone::getRegion(): intltz_get_region: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getRegion() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlTimeZone::getRegion(): intltz_get_region: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getRegion(): intltz_get_region: could not convert time zone id to UTF-16 in %s on line %d
bool(false)

Warning: IntlTimeZone::getRegion(): intltz_get_region: Error obtaining region in %s on line %d
bool(false)
