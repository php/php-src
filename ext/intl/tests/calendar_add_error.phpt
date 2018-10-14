--TEST--
IntlCalendar::add(): bad arguments
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

var_dump($c->add(1, 2, 3));
var_dump($c->add(-1, 2));
var_dump($c->add(1));

var_dump(intlcal_add($c, 1, 2, 3));
var_dump(intlcal_add(1, 2, 3));
--EXPECTF--
Warning: IntlCalendar::add() expects exactly 2 parameters, 3 given in %s on line %d

Warning: IntlCalendar::add(): intlcal_add: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::add(): intlcal_add: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::add() expects exactly 2 parameters, 1 given in %s on line %d

Warning: IntlCalendar::add(): intlcal_add: bad arguments in %s on line %d
bool(false)

Warning: intlcal_add() expects exactly 3 parameters, 4 given in %s on line %d

Warning: intlcal_add(): intlcal_add: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_add() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_add(1, 2, 3)
#1 {main}
  thrown in %s on line %d
