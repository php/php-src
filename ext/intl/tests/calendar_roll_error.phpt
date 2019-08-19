--TEST--
IntlCalendar::roll(): bad arguments
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

var_dump($c->roll(-1, 2));

var_dump(intlcal_roll(1, 2, 3));
--EXPECTF--
Warning: IntlCalendar::roll(): intlcal_roll: invalid field in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: intlcal_roll() expects parameter 1 to be IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_roll(1, 2, 3)
#1 {main}
  thrown in %s on line %d
