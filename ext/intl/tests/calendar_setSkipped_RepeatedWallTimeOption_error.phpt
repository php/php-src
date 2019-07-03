--TEST--
IntlCalendar::setSkipped/RepeatedWallTimeOption(): bad arguments
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

var_dump($c->setSkippedWallTimeOption(3));
var_dump($c->setRepeatedWallTimeOption(2));

var_dump(intlcal_set_repeated_wall_time_option(1, 1));
--EXPECTF--
Warning: IntlCalendar::setSkippedWallTimeOption(): intlcal_set_skipped_wall_time_option: invalid option in %s on line %d
bool(false)

Warning: IntlCalendar::setRepeatedWallTimeOption(): intlcal_set_repeated_wall_time_option: invalid option in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: intlcal_set_repeated_wall_time_option() expects parameter 1 to be IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_set_repeated_wall_time_option(1, 1)
#1 {main}
  thrown in %s on line %d
