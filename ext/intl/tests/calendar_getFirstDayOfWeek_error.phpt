--TEST--
IntlCalendar::getFirstDayOfWeek(): bad arguments
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

var_dump($c->getFirstDayOfWeek(1));

var_dump(intlcal_get_first_day_of_week($c, 1));
var_dump(intlcal_get_first_day_of_week(1));

--EXPECTF--

Warning: IntlCalendar::getFirstDayOfWeek() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::getFirstDayOfWeek(): intlcal_get_first_day_of_week: bad arguments in %s on line %d
bool(false)

Warning: intlcal_get_first_day_of_week() expects exactly 1 parameter, 2 given in %s on line %d

Warning: intlcal_get_first_day_of_week(): intlcal_get_first_day_of_week: bad arguments in %s on line %d
bool(false)

Catchable fatal error: Argument 1 passed to intlcal_get_first_day_of_week() must be an instance of IntlCalendar, integer given in %s on line %d
