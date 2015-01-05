--TEST--
IntlTimeZone::useDaylightTime(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->useDaylightTime('foo'));
intltz_use_daylight_time(null);

--EXPECTF--

Warning: IntlTimeZone::useDaylightTime() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::useDaylightTime(): intltz_use_daylight_time: bad arguments in %s on line %d
bool(false)

Catchable fatal error: Argument 1 passed to intltz_use_daylight_time() must be an instance of IntlTimeZone, null given in %s on line %d
