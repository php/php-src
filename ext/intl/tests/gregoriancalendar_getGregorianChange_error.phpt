--TEST--
IntlGregorianCalendar::getGregorianChange(): bad arguments
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
var_dump($c->getGregorianChange(1));

var_dump(intlgregcal_get_gregorian_change($c, 1));
var_dump(intlgregcal_get_gregorian_change(1));
--EXPECTF--

Warning: IntlGregorianCalendar::getGregorianChange() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlGregorianCalendar::getGregorianChange(): intlgregcal_get_gregorian_change: bad arguments in %s on line %d
bool(false)

Warning: intlgregcal_get_gregorian_change() expects exactly 1 parameter, 2 given in %s on line %d

Warning: intlgregcal_get_gregorian_change(): intlgregcal_get_gregorian_change: bad arguments in %s on line %d
bool(false)

Fatal error: Argument 1 passed to intlgregcal_get_gregorian_change() must be an instance of IntlGregorianCalendar, integer given in %s on line %d
