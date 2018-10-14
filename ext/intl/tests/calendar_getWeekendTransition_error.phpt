--TEST--
IntlCalendar::getWeekendTransition(): bad arguments
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

var_dump($c->getWeekendTransition());
var_dump($c->getWeekendTransition(1, 2));
var_dump($c->getWeekendTransition(0));

var_dump(intlcal_get_weekend_transition($c));
var_dump(intlcal_get_weekend_transition(1, 1));
--EXPECTF--
Warning: IntlCalendar::getWeekendTransition() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getWeekendTransition(): intlcal_get_weekend_transition: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getWeekendTransition() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::getWeekendTransition(): intlcal_get_weekend_transition: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getWeekendTransition(): intlcal_get_weekend_transition: invalid day of week in %s on line %d
bool(false)

Warning: intlcal_get_weekend_transition() expects exactly 2 parameters, 1 given in %s on line %d

Warning: intlcal_get_weekend_transition(): intlcal_get_weekend_transition: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_get_weekend_transition() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_get_weekend_transition(1, 1)
#1 {main}
  thrown in %s on line %d
