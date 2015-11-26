--TEST--
IntlGregorianCalendar::setGregorianChange(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar();
var_dump($c->setGregorianChange());
var_dump($c->setGregorianChange(1, 2));
var_dump($c->setGregorianChange("sdfds"));

var_dump(intlgregcal_set_gregorian_change($c));
var_dump(intlgregcal_set_gregorian_change(1, 4.));
--EXPECTF--

Warning: IntlGregorianCalendar::setGregorianChange() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlGregorianCalendar::setGregorianChange(): intlgregcal_set_gregorian_change: bad arguments in %s on line %d
bool(false)

Warning: IntlGregorianCalendar::setGregorianChange() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlGregorianCalendar::setGregorianChange(): intlgregcal_set_gregorian_change: bad arguments in %s on line %d
bool(false)

Warning: IntlGregorianCalendar::setGregorianChange() expects parameter 1 to be float, string given in %s on line %d

Warning: IntlGregorianCalendar::setGregorianChange(): intlgregcal_set_gregorian_change: bad arguments in %s on line %d
bool(false)

Warning: intlgregcal_set_gregorian_change() expects exactly 2 parameters, 1 given in %s on line %d

Warning: intlgregcal_set_gregorian_change(): intlgregcal_set_gregorian_change: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intlgregcal_set_gregorian_change() must be an instance of IntlGregorianCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlgregcal_set_gregorian_change(1, 4)
#1 {main}
  thrown in %s on line %d
