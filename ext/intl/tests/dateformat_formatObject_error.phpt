--TEST--
IntlDateFormatter::formatObject(): error conditions
--EXTENSIONS--
intl
--INI--
intl.default_locale=pt_PT
date.timezone=Europe/Lisbon
--FILE--
<?php

class A extends IntlCalendar {function __construct(){}}

class B extends DateTime {function __construct(){}}

$cal = IntlCalendar::createInstance();

try {
    var_dump(IntlDateFormatter::formatObject(new B));
    var_dump(intl_get_error_message());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(IntlDateFormatter::formatObject(new stdclass));
var_dump(intl_get_error_message());
var_dump(IntlDateFormatter::formatObject(new A));
var_dump(intl_get_error_message());

var_dump(IntlDateFormatter::formatObject($cal, -2));
var_dump(intl_get_error_message());
var_dump(IntlDateFormatter::formatObject($cal, array()));
var_dump(intl_get_error_message());
var_dump(IntlDateFormatter::formatObject($cal, array(1,2,3)));
var_dump(intl_get_error_message());
var_dump(IntlDateFormatter::formatObject($cal, array(array(), 1)));
var_dump(intl_get_error_message());
var_dump(IntlDateFormatter::formatObject($cal, array(1, -2)));
var_dump(intl_get_error_message());
var_dump(IntlDateFormatter::formatObject($cal, ""));
var_dump(intl_get_error_message());

?>
--EXPECT--
DateObjectError: Object of type B (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
bool(false)
string(130) "IntlDateFormatter::formatObject(): the passed object must be an instance of either IntlCalendar or DateTimeInterface: U_ZERO_ERROR"
bool(false)
string(112) "IntlDateFormatter::formatObject(): bad IntlCalendar instance: not initialized properly: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(97) "IntlDateFormatter::formatObject(): the date/time format type is invalid: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(108) "IntlDateFormatter::formatObject(): bad format; if array, it must have two elements: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(108) "IntlDateFormatter::formatObject(): bad format; if array, it must have two elements: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(130) "IntlDateFormatter::formatObject(): bad format; the date format (first element of the array) is not valid: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(131) "IntlDateFormatter::formatObject(): bad format; the time format (second element of the array) is not valid: U_ILLEGAL_ARGUMENT_ERROR"
bool(false)
string(80) "IntlDateFormatter::formatObject(): the format is empty: U_ILLEGAL_ARGUMENT_ERROR"
