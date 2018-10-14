--TEST--
IntlCalendar::getNow(): bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlcal_get_now(1));
var_dump(IntlCalendar::getNow(2));
--EXPECTF--
Warning: intlcal_get_now() expects exactly 0 parameters, 1 given in %s on line %d

Warning: intlcal_get_now(): intlcal_get_now: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getNow() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::getNow(): intlcal_get_now: bad arguments in %s on line %d
bool(false)
