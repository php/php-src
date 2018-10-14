--TEST--
IntlCalendar::getSkipped/RepeatedWallTimeOption(): bad arguments
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

var_dump($c->getSkippedWallTimeOption(1));
var_dump($c->getRepeatedWallTimeOption(1));

var_dump(intlcal_get_skipped_wall_time_option($c, 1));
var_dump(intlcal_get_repeated_wall_time_option($c, 1));

var_dump(intlcal_get_skipped_wall_time_option(1));
--EXPECTF--
Warning: IntlCalendar::getSkippedWallTimeOption() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::getSkippedWallTimeOption(): intlcal_get_skipped_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getRepeatedWallTimeOption() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::getRepeatedWallTimeOption(): intlcal_get_repeated_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: intlcal_get_skipped_wall_time_option() expects exactly 1 parameter, 2 given in %s on line %d

Warning: intlcal_get_skipped_wall_time_option(): intlcal_get_skipped_wall_time_option: bad arguments in %s on line %d
bool(false)

Warning: intlcal_get_repeated_wall_time_option() expects exactly 1 parameter, 2 given in %s on line %d

Warning: intlcal_get_repeated_wall_time_option(): intlcal_get_repeated_wall_time_option: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_get_skipped_wall_time_option() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_get_skipped_wall_time_option(1)
#1 {main}
  thrown in %s on line %d
