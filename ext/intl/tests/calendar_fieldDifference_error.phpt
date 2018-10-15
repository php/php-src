--TEST--
IntlCalendar::fieldDifference(): bad arguments
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

var_dump($c->fieldDifference($c, 2, 3));
var_dump($c->fieldDifference(INF, 2));
var_dump($c->fieldDifference(1));

var_dump(intlcal_field_difference($c, 0, 1, 2));
var_dump(intlcal_field_difference(1, 0, 1));
--EXPECTF--
Warning: IntlCalendar::fieldDifference() expects exactly 2 parameters, 3 given in %s on line %d

Warning: IntlCalendar::fieldDifference(): intlcal_field_difference: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::fieldDifference(): intlcal_field_difference: Call to ICU method has failed in %s on line %d
bool(false)

Warning: IntlCalendar::fieldDifference() expects exactly 2 parameters, 1 given in %s on line %d

Warning: IntlCalendar::fieldDifference(): intlcal_field_difference: bad arguments in %s on line %d
bool(false)

Warning: intlcal_field_difference() expects exactly 3 parameters, 4 given in %s on line %d

Warning: intlcal_field_difference(): intlcal_field_difference: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_field_difference() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_field_difference(1, 0, 1)
#1 {main}
  thrown in %s on line %d
