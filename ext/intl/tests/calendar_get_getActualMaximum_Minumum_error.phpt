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

var_dump($c->get(-1));
var_dump($c->getActualMaximum(-1));
var_dump($c->getActualMinimum(-1));

?>
--EXPECTF--
Warning: IntlCalendar::get(): intlcal_get: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMaximum(): intlcal_get_actual_maximum: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::getActualMinimum(): intlcal_get_actual_minimum: invalid field in %s on line %d
bool(false)
