--TEST--
IntlCalendar::isWeekend basic test
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->isWeekend(strtotime('2012-02-29 12:00:00 +0000') * 1000));
var_dump(intlcal_is_weekend($intlcal, strtotime('2012-02-29 12:00:00 +0000') * 1000));
var_dump($intlcal->isWeekend(strtotime('2012-03-11 12:00:00 +0000') * 1000));
?>
--EXPECT--
bool(false)
bool(false)
bool(true)
