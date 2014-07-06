--TEST--
IntlDateFormatter::getCalendarObject(): bad args
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", 'Atlantic/Azores');

$df = new IntlDateFormatter(NULL, 0, 0);

var_dump($df->getCalendarObject(9));
var_dump(datefmt_get_calendar_object($df, 9));
var_dump(datefmt_get_calendar_object($df, 9));
var_dump(datefmt_get_calendar_object(new stdclass));

?>
==DONE==
--EXPECTF--

Warning: IntlDateFormatter::getCalendarObject() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlDateFormatter::getCalendarObject(): datefmt_get_calendar_object: unable to parse input params in %s on line %d
bool(false)

Warning: datefmt_get_calendar_object() expects exactly 1 parameter, 2 given in %s on line %d

Warning: datefmt_get_calendar_object(): datefmt_get_calendar_object: unable to parse input params in %s on line %d
bool(false)

Warning: datefmt_get_calendar_object() expects exactly 1 parameter, 2 given in %s on line %d

Warning: datefmt_get_calendar_object(): datefmt_get_calendar_object: unable to parse input params in %s on line %d
bool(false)

Warning: datefmt_get_calendar_object() expects parameter 1 to be IntlDateFormatter, object given in %s on line %d

Warning: datefmt_get_calendar_object(): datefmt_get_calendar_object: unable to parse input params in %s on line %d
bool(false)
==DONE==
