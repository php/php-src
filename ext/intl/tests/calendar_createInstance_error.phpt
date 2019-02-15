--TEST--
IntlCalendar::createInstance: bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

class X extends IntlTimeZone {
function __construct() {}
}

var_dump(IntlCalendar::createInstance(1, 2, 3));
var_dump(intlcal_create_instance(1, 2, 3));
var_dump(intlcal_create_instance(new X, NULL));
var_dump(intlcal_create_instance(NULL, array()));
--EXPECTF--
Warning: IntlCalendar::createInstance() expects at most 2 parameters, 3 given in %s on line %d

Warning: IntlCalendar::createInstance(): intlcal_create_calendar: bad arguments in %s on line %d
NULL

Warning: intlcal_create_instance() expects at most 2 parameters, 3 given in %s on line %d

Warning: intlcal_create_instance(): intlcal_create_calendar: bad arguments in %s on line %d
NULL

Warning: intlcal_create_instance(): intlcal_create_instance: passed IntlTimeZone is not properly constructed in %s on line %d
NULL

Warning: intlcal_create_instance() expects parameter 2 to be string, array given in %s on line %d

Warning: intlcal_create_instance(): intlcal_create_calendar: bad arguments in %s on line %d
NULL
