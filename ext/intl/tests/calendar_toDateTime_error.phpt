--TEST--
IntlCalendar::toDateTime(): bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set('date.timezone', 'Europe/Lisbon');

$cal = new IntlGregorianCalendar();
var_dump($cal->toDateTime(3));

var_dump(intlcal_to_date_time($cal, 3));

$cal = new IntlGregorianCalendar("Etc/Unknown");
try {
var_dump($cal->toDateTime());
} catch (Exception $e) {
var_dump("exception: {$e->getMessage()}");
}

var_dump(intlcal_to_date_time(3));

--EXPECTF--

Warning: IntlCalendar::toDateTime() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::toDateTime(): intlcal_to_date_time: bad arguments in %s on line %d
bool(false)

Warning: intlcal_to_date_time() expects exactly 1 parameter, 2 given in %s on line %d

Warning: intlcal_to_date_time(): intlcal_to_date_time: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::toDateTime(): intlcal_to_date_time: DateTimeZone constructor threw exception in %s on line %d
string(77) "exception: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)"

Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_to_date_time() must be an instance of IntlCalendar, integer given in %s:%d
Stack trace:
#0 %s(%d): intlcal_to_date_time(3)
#1 {main}
  thrown in %s on line %d
