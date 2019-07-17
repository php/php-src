--TEST--
IntlCalendar::clear(): bad arguments
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

var_dump($c->clear(-1));

var_dump(intlcal_clear($c, -1));
var_dump(intlcal_clear(1, 2));
--EXPECTF--
Warning: IntlCalendar::clear(): intlcal_clear: invalid field in %s on line %d
bool(false)

Warning: intlcal_clear(): intlcal_clear: invalid field in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: intlcal_clear() expects parameter 1 to be IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_clear(1, 2)
#1 {main}
  thrown in %s on line %d
