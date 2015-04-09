--TEST--
IntlCalendar::setFirstDayOfWeek(): bad arguments
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

var_dump($c->setFirstDayOfWeek());
var_dump($c->setFirstDayOfWeek(1, 2));
var_dump($c->setFirstDayOfWeek(0));

var_dump(intlcal_set_first_day_of_week($c, 0));
var_dump(intlcal_set_first_day_of_week(1, 2));

--EXPECTF--

Warning: IntlCalendar::setFirstDayOfWeek() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::setFirstDayOfWeek(): intlcal_set_first_day_of_week: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setFirstDayOfWeek() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::setFirstDayOfWeek(): intlcal_set_first_day_of_week: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setFirstDayOfWeek(): intlcal_set_first_day_of_week: invalid day of week in %s on line %d
bool(false)

Warning: intlcal_set_first_day_of_week(): intlcal_set_first_day_of_week: invalid day of week in %s on line %d
bool(false)

Fatal error: Argument 1 passed to intlcal_set_first_day_of_week() must be an instance of IntlCalendar, integer given in %s on line %d
