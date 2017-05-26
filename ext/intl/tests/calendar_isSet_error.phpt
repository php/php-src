--TEST--
IntlCalendar::isSet(): bad arguments
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

var_dump($c->isSet());
var_dump($c->isSet(1, 2));
var_dump($c->isSet(-1));

var_dump(intlcal_is_set($c));
var_dump(intlcal_is_set(1, 2));

--EXPECTF--

Warning: IntlCalendar::isSet() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::isSet(): intlcal_is_set: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::isSet() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::isSet(): intlcal_is_set: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::isSet(): intlcal_is_set: invalid field in %s on line %d
bool(false)

Warning: intlcal_is_set() expects exactly 2 parameters, 1 given in %s on line %d

Warning: intlcal_is_set(): intlcal_is_set: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_is_set() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_is_set(1, 2)
#1 {main}
  thrown in %s on line %d
