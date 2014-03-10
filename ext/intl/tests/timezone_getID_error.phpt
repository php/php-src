--TEST--
IntlTimeZone::getID(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getID('foo'));
intltz_get_id(null);


--EXPECTF--

Warning: IntlTimeZone::getID() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getID(): intltz_get_id: bad arguments in %s on line %d
bool(false)

Catchable fatal error: Argument 1 passed to intltz_get_id() must be an instance of IntlTimeZone, null given in %s on line %d
