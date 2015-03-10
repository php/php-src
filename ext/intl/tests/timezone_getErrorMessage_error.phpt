--TEST--
IntlTimeZone::getErrorMessage(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getErrorMessage(array()));

var_dump(intltz_get_error_message(null));

--EXPECTF--

Warning: IntlTimeZone::getErrorMessage() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getErrorMessage(): intltz_get_error_message: bad arguments in %s on line %d
bool(false)

Fatal error: Argument 1 passed to intltz_get_error_message() must be an instance of IntlTimeZone, null given in %s on line %d
