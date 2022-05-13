--TEST--
IntlCalendar::fromDateTime(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");
date_default_timezone_set('Europe/Lisbon');

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
?>
--EXPECTF--
threw exception, OK
Warning: IntlCalendar::fromDateTime(): intlcal_from_date_time: DateTime object is unconstructed in %s on line %d
NULL

Warning: IntlCalendar::fromDateTime(): intlcal_from_date_time: object has an time zone offset that's too large in %s on line %d
NULL

Warning: IntlCalendar::fromDateTime(): intlcal_from_date_time: time zone id 'WEST' extracted from ext/date DateTimeZone not recognized in %s on line %d
NULL
