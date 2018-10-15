--TEST--
IntlTimeZone::getErrorCode(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getErrorCode(array()));

var_dump(intltz_get_error_code(null));
--EXPECTF--
Warning: IntlTimeZone::getErrorCode() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getErrorCode(): intltz_get_error_code: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intltz_get_error_code() must be an instance of IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_get_error_code(NULL)
#1 {main}
  thrown in %s on line %d
