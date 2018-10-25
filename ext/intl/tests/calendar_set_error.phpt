--TEST--
IntlCalendar::set(): bad arguments
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

var_dump($c->set(1));
var_dump($c->set(1, 2, 3, 4));
var_dump($c->set(1, 2, 3, 4, 5, 6, 7));
var_dump($c->set(-1, 2));

var_dump(intlcal_set($c, -1, 2));
var_dump(intlcal_set(1, 2, 3));
--EXPECTF--
Warning: IntlCalendar::set() expects at least 2 parameters, 1 given in %s on line %d

Warning: IntlCalendar::set(): intlcal_set: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::set(): intlcal_set: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::set(): intlcal_set: too many arguments in %s on line %d
bool(false)

Warning: IntlCalendar::set(): intlcal_set: invalid field in %s on line %d
bool(false)

Warning: intlcal_set(): intlcal_set: invalid field in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_set() must be an instance of IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_set(1, 2, 3)
#1 {main}
  thrown in %s on line %d
