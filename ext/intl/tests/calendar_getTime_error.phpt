--TEST--
IntlCalendar::getTime(): bad arguments
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

var_dump($c->getTime(1));

var_dump(intlcal_get_time($c, 1));
var_dump(intlcal_get_time(1));
--EXPECTF--

Warning: IntlCalendar::getTime() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::getTime(): intlcal_get_time: bad arguments in %s on line %d
bool(false)

Warning: intlcal_get_time() expects exactly 1 parameter, 2 given in %s on line %d

Warning: intlcal_get_time(): intlcal_get_time: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_get_time() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_get_time(1)
#1 {main}
  thrown in %s on line %d
