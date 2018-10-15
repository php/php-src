--TEST--
IntlCalendar::getDayOfWeekOfType(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.4') < 0)
	die('skip for ICU 4.4+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

var_dump($c->getDayOfWeekType(1, 2));
var_dump($c->getDayOfWeekType(0));
var_dump($c->getDayOfWeekType());

var_dump(intlcal_get_day_of_week_type($c, "foo"));
var_dump(intlcal_get_day_of_week_type(1, 1));
--EXPECTF--
Warning: IntlCalendar::getDayOfWeekType() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::getDayOfWeekType(): intlcal_get_day_of_week_type: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getDayOfWeekType(): intlcal_get_day_of_week_type: invalid day of week in %s on line %d
bool(false)

Warning: IntlCalendar::getDayOfWeekType() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getDayOfWeekType(): intlcal_get_day_of_week_type: bad arguments in %s on line %d
bool(false)

Warning: intlcal_get_day_of_week_type() expects parameter 2 to be int, string given in %s on line %d

Warning: intlcal_get_day_of_week_type(): intlcal_get_day_of_week_type: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_get_day_of_week_type() must be an instance of IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_get_day_of_week_type(1, 1)
#1 {main}
  thrown in %s on line %d
