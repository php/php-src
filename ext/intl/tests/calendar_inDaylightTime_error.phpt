--TEST--
IntlCalendar::inDaylightTime(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

var_dump($c->inDaylightTime(1));

var_dump(intlcal_in_daylight_time($c, 1));
var_dump(intlcal_in_daylight_time(1));
--EXPECTF--
Warning: IntlCalendar::inDaylightTime() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::inDaylightTime(): intlcal_in_daylight_time: bad arguments in %s on line %d
bool(false)

Warning: intlcal_in_daylight_time() expects exactly 1 parameter, 2 given in %s on line %d

Warning: intlcal_in_daylight_time(): intlcal_in_daylight_time: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_in_daylight_time() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_in_daylight_time(1)
#1 {main}
  thrown in %s on line %d
