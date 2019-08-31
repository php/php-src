--TEST--
IntlCalendar::setMinimalDaysInFirstWeek(): bad arguments
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

var_dump($c->setMinimalDaysInFirstWeek(0));

var_dump(intlcal_set_minimal_days_in_first_week($c, 0));
var_dump(intlcal_set_minimal_days_in_first_week(1, 2));
--EXPECTF--
Warning: IntlCalendar::setMinimalDaysInFirstWeek(): intlcal_set_minimal_days_in_first_week: invalid number of days; must be between 1 and 7 in %s on line %d
bool(false)

Warning: intlcal_set_minimal_days_in_first_week(): intlcal_set_minimal_days_in_first_week: invalid number of days; must be between 1 and 7 in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: intlcal_set_minimal_days_in_first_week() expects parameter 1 to be IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_set_minimal_days_in_first_week(1, 2)
#1 {main}
  thrown in %s on line %d
