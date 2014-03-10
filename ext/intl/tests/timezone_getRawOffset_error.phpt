--TEST--
IntlTimeZone::getRawOffset(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getRawOffset('foo'));

intltz_get_raw_offset(null);

--EXPECTF--

Warning: IntlTimeZone::getRawOffset() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getRawOffset(): intltz_get_raw_offset: bad arguments in %s on line %d
bool(false)

Catchable fatal error: Argument 1 passed to intltz_get_raw_offset() must be an instance of IntlTimeZone, null given in %s on line %d
