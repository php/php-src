--TEST--
IntlGregorianCalendar::isLeapYear(): basic test
--EXTENSIONS--
intl
--INI--
date.timezone=Europe/Amsterdam
intl.default_locale=nl
--FILE--
<?php

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
