--TEST--
IntlGregorianCalendar::isLeapYear(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
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
==DONE==
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
==DONE==