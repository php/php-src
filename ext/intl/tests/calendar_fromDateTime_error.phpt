--TEST--
IntlCalendar::fromDateTime(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");
date_default_timezone_set('Europe/Lisbon');

var_dump(IntlCalendar::fromDateTime());
var_dump(IntlCalendar::fromDateTime(0,1,2));

try {
IntlCalendar::fromDateTime("foobar");
} catch (Exception $e) {
	echo "threw exception, OK";
}
class A extends DateTime {
function __construct() {}
}

var_dump(IntlCalendar::fromDateTime(new A));

$date = new DateTime('2012-01-01 00:00:00 +24:00');
var_dump(IntlCalendar::fromDateTime($date));

$date = new DateTime('2012-01-01 00:00:00 WEST');
var_dump(IntlCalendar::fromDateTime($date));

var_dump(intlcal_from_date_time());
--EXPECTF--
Warning: IntlCalendar::fromDateTime() expects at least 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::fromDateTime(): intlcal_from_date_time: bad arguments in %s on line %d
NULL

Warning: IntlCalendar::fromDateTime() expects at most 2 parameters, 3 given in %s on line %d

Warning: IntlCalendar::fromDateTime(): intlcal_from_date_time: bad arguments in %s on line %d
NULL
threw exception, OK
Warning: IntlCalendar::fromDateTime(): intlcal_from_date_time: DateTime object is unconstructed in %s on line %d
NULL

Warning: IntlCalendar::fromDateTime(): intlcal_from_date_time: object has an time zone offset that's too large in %s on line %d
NULL

Warning: IntlCalendar::fromDateTime(): intlcal_from_date_time: time zone id 'WEST' extracted from ext/date DateTimeZone not recognized in %s on line %d
NULL

Warning: intlcal_from_date_time() expects at least 1 parameter, 0 given in %s on line %d

Warning: intlcal_from_date_time(): intlcal_from_date_time: bad arguments in %s on line %d
NULL
