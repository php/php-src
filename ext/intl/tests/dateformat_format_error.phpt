--TEST--
IntlDateFormatter->format() errors
--EXTENSIONS--
intl
--FILE--
<?php

$locale = 'en_US';
$timezone = 'GMT-10:00';
$type = IntlDateFormatter::FULL;
$object = new stdClass();

$f = new IntlDateFormatter($locale, $type, $type, $timezone, IntlDateFormatter::GREGORIAN);

$v = $f->format($object);
var_dump($v);
var_dump(intl_get_error_message());

$v = datefmt_format($f, $object);
var_dump($v);
var_dump(intl_get_error_message());

?>
--EXPECT--
bool(false)
string(127) "datefmt_format: invalid object type for date/time (only IntlCalendar and DateTimeInterface permitted): U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(127) "datefmt_format: invalid object type for date/time (only IntlCalendar and DateTimeInterface permitted): U_ILLEGAL_ARGUMENT_ERROR"
