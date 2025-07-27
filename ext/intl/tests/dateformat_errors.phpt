--TEST--
IntlDateFormatter with invalid locale
--EXTENSIONS--
intl
--FILE--
<?php

$locale = 'en_US';
$timezone = 'GMT+05:12';
$type = IntlDateFormatter::FULL;
$invalidCalendar = -1;

try {
    $df = new IntlDateFormatter($locale, $type, $type, $timezone, $invalidCalendar);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$df = IntlDateFormatter::create($locale, $type, $type, $timezone, $invalidCalendar);
var_dump($df);
var_dump(intl_get_error_message());

$df = datefmt_create($locale, $type, $type, $timezone, $invalidCalendar);
var_dump($df);
var_dump(intl_get_error_message());

?>
--EXPECT--
IntlException: datefmt_create: Invalid value for calendar type; it must be one of IntlDateFormatter::TRADITIONAL (locale's default calendar) or IntlDateFormatter::GREGORIAN. Alternatively, it can be an IntlCalendar object: U_ILLEGAL_ARGUMENT_ERROR
NULL
string(232) "datefmt_create: Invalid value for calendar type; it must be one of IntlDateFormatter::TRADITIONAL (locale's default calendar) or IntlDateFormatter::GREGORIAN. Alternatively, it can be an IntlCalendar object: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(232) "datefmt_create: Invalid value for calendar type; it must be one of IntlDateFormatter::TRADITIONAL (locale's default calendar) or IntlDateFormatter::GREGORIAN. Alternatively, it can be an IntlCalendar object: U_ILLEGAL_ARGUMENT_ERROR"
