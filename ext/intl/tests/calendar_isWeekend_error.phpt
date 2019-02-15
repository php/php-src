--TEST--
IntlCalendar::isWeekend(): bad arguments
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

var_dump($c->isWeekend(1, 2));
var_dump($c->isWeekend("jhhk"));

var_dump(intlcal_is_weekend($c, "jj"));
var_dump(intlcal_is_weekend(1));
--EXPECTF--
Warning: IntlCalendar::isWeekend() expects at most 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::isWeekend(): intlcal_is_weekend: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::isWeekend() expects parameter 1 to be float, string given in %s on line %d

Warning: IntlCalendar::isWeekend(): intlcal_is_weekend: bad arguments in %s on line %d
bool(false)

Warning: intlcal_is_weekend() expects parameter 2 to be float, string given in %s on line %d

Warning: intlcal_is_weekend(): intlcal_is_weekend: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_is_weekend() must be an instance of IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_is_weekend(1)
#1 {main}
  thrown in %s on line %d
