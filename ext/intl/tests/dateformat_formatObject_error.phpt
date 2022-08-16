--TEST--
IntlDateFormatter::formatObject(): error conditions
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");
ini_set("date.timezone", "Europe/Lisbon");

var_dump(IntlDateFormatter::formatObject(new stdclass));

class A extends IntlCalendar {function __construct(){}}
var_dump(IntlDateFormatter::formatObject(new A));
class B extends DateTime {function __construct(){}}
try {
    var_dump(IntlDateFormatter::formatObject(new B));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$cal = IntlCalendar::createInstance();
var_dump(IntlDateFormatter::formatObject($cal, -2));
var_dump(IntlDateFormatter::formatObject($cal, array()));
var_dump(IntlDateFormatter::formatObject($cal, array(1,2,3)));
var_dump(IntlDateFormatter::formatObject($cal, array(array(), 1)));
var_dump(IntlDateFormatter::formatObject($cal, array(1, -2)));
var_dump(IntlDateFormatter::formatObject($cal, ""));

?>
--EXPECTF--
Warning: IntlDateFormatter::formatObject(): datefmt_format_object: the passed object must be an instance of either IntlCalendar or DateTimeInterface in %s on line %d
bool(false)

Warning: IntlDateFormatter::formatObject(): datefmt_format_object: bad IntlCalendar instance: not initialized properly in %s on line %d
bool(false)

Warning: IntlDateFormatter::formatObject(): datefmt_format_object: error calling ::getTimeStamp() on the object in %s on line %d
The DateTime object has not been correctly initialized by its constructor

Warning: IntlDateFormatter::formatObject(): datefmt_format_object: the date/time format type is invalid in %s on line %d
bool(false)

Warning: IntlDateFormatter::formatObject(): datefmt_format_object: bad format; if array, it must have two elements in %s on line %d
bool(false)

Warning: IntlDateFormatter::formatObject(): datefmt_format_object: bad format; if array, it must have two elements in %s on line %d
bool(false)

Warning: IntlDateFormatter::formatObject(): datefmt_format_object: bad format; the date format (first element of the array) is not valid in %s on line %d
bool(false)

Warning: IntlDateFormatter::formatObject(): datefmt_format_object: bad format; the time format (second element of the array) is not valid in %s on line %d
bool(false)

Warning: IntlDateFormatter::formatObject(): datefmt_format_object: the format is empty in %s on line %d
bool(false)
