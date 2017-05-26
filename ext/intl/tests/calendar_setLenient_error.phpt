--TEST--
IntlCalendar::setLenient(): bad arguments
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

var_dump($c->setLenient());
var_dump($c->setLenient(array()));
var_dump($c->setLenient(1, 2));

var_dump(intlcal_set_lenient($c, array()));
var_dump(intlcal_set_lenient(1, false));

--EXPECTF--

Warning: IntlCalendar::setLenient() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::setLenient(): intlcal_set_lenient: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setLenient() expects parameter 1 to be boolean, array given in %s on line %d

Warning: IntlCalendar::setLenient(): intlcal_set_lenient: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::setLenient() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::setLenient(): intlcal_set_lenient: bad arguments in %s on line %d
bool(false)

Warning: intlcal_set_lenient() expects parameter 2 to be boolean, array given in %s on line %d

Warning: intlcal_set_lenient(): intlcal_set_lenient: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_set_lenient() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_set_lenient(1, false)
#1 {main}
  thrown in %s on line %d
