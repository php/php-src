--TEST--
IntlTimeZone::getDSTSavings(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getDSTSavings(array()));

var_dump(intltz_get_dst_savings(null));
--EXPECTF--
Warning: IntlTimeZone::getDSTSavings() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getDSTSavings(): intltz_get_dst_savings: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intltz_get_dst_savings() must be an instance of IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_get_dst_savings(NULL)
#1 {main}
  thrown in %s on line %d
