--TEST--
IntlCalendar::setSkipped/RepeatedWallTimeOption(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '49') < 0)
	die('skip for ICU 49+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

var_dump($c->setSkippedWallTimeOption());
var_dump($c->setRepeatedWallTimeOption());

var_dump($c->setSkippedWallTimeOption(1, 2));
var_dump($c->setRepeatedWallTimeOption(1, 2));

var_dump($c->setSkippedWallTimeOption(array()));
var_dump($c->setRepeatedWallTimeOption(array()));

var_dump($c->setSkippedWallTimeOption(3));
var_dump($c->setRepeatedWallTimeOption(2));

var_dump(intlcal_set_skipped_wall_time_option($c));
var_dump(intlcal_set_repeated_wall_time_option($c));

var_dump(intlcal_set_repeated_wall_time_option(1, 1));
--EXPECTF--
Warning: IntlCalendar::setSkippedWallTimeOption() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::setSkippedWallTimeOption(): intlcal_set_skipped_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setRepeatedWallTimeOption() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::setRepeatedWallTimeOption(): intlcal_set_repeated_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setSkippedWallTimeOption() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::setSkippedWallTimeOption(): intlcal_set_skipped_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setRepeatedWallTimeOption() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::setRepeatedWallTimeOption(): intlcal_set_repeated_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setSkippedWallTimeOption() expects parameter 1 to be int, array given in %s on line %d

Warning: IntlCalendar::setSkippedWallTimeOption(): intlcal_set_skipped_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setRepeatedWallTimeOption() expects parameter 1 to be int, array given in %s on line %d

Warning: IntlCalendar::setRepeatedWallTimeOption(): intlcal_set_repeated_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setSkippedWallTimeOption(): intlcal_set_skipped_wall_time_option: invalid option in %s on line %d
bool(false)

Warning: IntlCalendar::setRepeatedWallTimeOption(): intlcal_set_repeated_wall_time_option: invalid option in %s on line %d
bool(false)

Warning: intlcal_set_skipped_wall_time_option() expects exactly 2 parameters, 1 given in %s on line %d

Warning: intlcal_set_skipped_wall_time_option(): intlcal_set_skipped_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: intlcal_set_repeated_wall_time_option() expects exactly 2 parameters, 1 given in %s on line %d

Warning: intlcal_set_repeated_wall_time_option(): intlcal_set_repeated_wall_time_option: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_set_repeated_wall_time_option() must be an instance of IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_set_repeated_wall_time_option(1, 1)
#1 {main}
  thrown in %s on line %d
