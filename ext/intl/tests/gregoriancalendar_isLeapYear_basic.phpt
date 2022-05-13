--TEST--
IntlGregorianCalendar::isLeapYear(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

date_default_timezone_set('Europe/Amsterdam');

$intlcal = new IntlGregorianCalendar();

var_dump($intlcal->isLeapYear(2012));
var_dump($intlcal->isLeapYear(1900));

var_dump(intlgregcal_is_leap_year($intlcal, 2012));
var_dump(intlgregcal_is_leap_year($intlcal, 1900));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)