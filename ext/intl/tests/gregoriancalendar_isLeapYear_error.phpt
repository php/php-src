--TEST--
IntlGregorianCalendar::isLeapYear(): bad arguments
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
var_dump($c->isLeapYear(2000, 2011));
var_dump($c->isLeapYear());
var_dump($c->isLeapYear("fgdf"));

var_dump(intlgregcal_is_leap_year($c, 1, 2));
var_dump(intlgregcal_is_leap_year($c));
var_dump(intlgregcal_is_leap_year(1, 2));
--EXPECTF--

Warning: IntlGregorianCalendar::isLeapYear() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlGregorianCalendar::isLeapYear(): intlgregcal_is_leap_year: bad arguments in %s on line %d
bool(false)

Warning: IntlGregorianCalendar::isLeapYear() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlGregorianCalendar::isLeapYear(): intlgregcal_is_leap_year: bad arguments in %s on line %d
bool(false)

Warning: IntlGregorianCalendar::isLeapYear() expects parameter 1 to be integer, string given in %s on line %d

Warning: IntlGregorianCalendar::isLeapYear(): intlgregcal_is_leap_year: bad arguments in %s on line %d
bool(false)

Warning: intlgregcal_is_leap_year() expects exactly 2 parameters, 3 given in %s on line %d

Warning: intlgregcal_is_leap_year(): intlgregcal_is_leap_year: bad arguments in %s on line %d
bool(false)

Warning: intlgregcal_is_leap_year() expects exactly 2 parameters, 1 given in %s on line %d

Warning: intlgregcal_is_leap_year(): intlgregcal_is_leap_year: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlgregcal_is_leap_year() must be an instance of IntlGregorianCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlgregcal_is_leap_year(1, 2)
#1 {main}
  thrown in %s on line %d
