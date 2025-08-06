--TEST--
IntlCalendar::getFirstDayOfWeek() basic test
--EXTENSIONS--
intl
--INI--
intl.default_locale=nl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->getFirstDayOfWeek());
var_dump(intlcal_get_first_day_of_week($intlcal));
?>
--EXPECT--
int(2)
int(2)
