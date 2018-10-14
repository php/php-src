--TEST--
IntlCalendar::get/getActualMaximum/getActualMinimum(): bad arguments
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

var_dump($c->get());
var_dump($c->getActualMaximum());
var_dump($c->getActualMinimum());

var_dump($c->get(-1));
var_dump($c->getActualMaximum(-1));
var_dump($c->getActualMinimum(-1));

var_dump($c->get("s"));
var_dump($c->getActualMaximum("s"));
var_dump($c->getActualMinimum("s"));

var_dump($c->get(1, 2));
var_dump($c->getActualMaximum(1, 2));
var_dump($c->getActualMinimum(1, 2));
--EXPECTF--
Warning: IntlCalendar::get() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::get(): intlcal_get: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMaximum() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getActualMaximum(): intlcal_get_actual_maximum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMinimum() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getActualMinimum(): intlcal_get_actual_minimum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::get(): intlcal_get: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMaximum(): intlcal_get_actual_maximum: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMinimum(): intlcal_get_actual_minimum: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::get() expects parameter 1 to be integer, string given in %s on line %d

Warning: IntlCalendar::get(): intlcal_get: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMaximum() expects parameter 1 to be integer, string given in %s on line %d

Warning: IntlCalendar::getActualMaximum(): intlcal_get_actual_maximum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMinimum() expects parameter 1 to be integer, string given in %s on line %d

Warning: IntlCalendar::getActualMinimum(): intlcal_get_actual_minimum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::get() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::get(): intlcal_get: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMaximum() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::getActualMaximum(): intlcal_get_actual_maximum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMinimum() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::getActualMinimum(): intlcal_get_actual_minimum: bad arguments in %s on line %d
bool(false)
