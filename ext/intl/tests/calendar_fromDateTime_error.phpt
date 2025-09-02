--TEST--
IntlCalendar::fromDateTime(): errors
--EXTENSIONS--
intl
--INI--
intl.default_locale=nl
date.timezone=Europe/Lisbon
--FILE--
<?php

try {
    IntlCalendar::fromDateTime("foobar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

class A extends DateTime {
	function __construct() {}
}

var_dump(IntlCalendar::fromDateTime(new A));
var_dump(intl_get_error_message());

$date = new DateTime('2012-01-01 00:00:00 +24:00');
var_dump(IntlCalendar::fromDateTime($date));
var_dump(intl_get_error_message());

$date = new DateTime('2012-01-01 00:00:00 WEST');
var_dump(IntlCalendar::fromDateTime($date));
var_dump(intl_get_error_message());

?>
--EXPECT--
DateMalformedStringException: Failed to parse time string (foobar) at position 0 (f): The timezone could not be found in the database
NULL
string(88) "IntlCalendar::fromDateTime(): DateTime object is unconstructed: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(103) "IntlCalendar::fromDateTime(): object has an time zone offset that's too large: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(127) "IntlCalendar::fromDateTime(): time zone id 'WEST' extracted from ext/date DateTimeZone not recognized: U_ILLEGAL_ARGUMENT_ERROR"
