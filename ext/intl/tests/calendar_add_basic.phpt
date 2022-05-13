--TEST--
IntlCalendar::add() basic test
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$time = strtotime('2012-02-29 00:00:00 +0000');
$time2 = strtotime('2012-03-01 05:06:07 +0000');

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->setTime($time * 1000);
$intlcal->add(IntlCalendar::FIELD_DAY_OF_MONTH, 1);
$intlcal->add(IntlCalendar::FIELD_HOUR, 5);
$intlcal->add(IntlCalendar::FIELD_MINUTE, 6);
intlcal_add($intlcal, IntlCalendar::FIELD_SECOND, 7);

var_dump(
    (float)$time2*1000,
    $intlcal->getTime());

?>
--EXPECT--
float(1330578367000)
float(1330578367000)