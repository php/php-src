--TEST--
IntlCalendar::getMinimalDaysInFirstWeek() basic test
--EXTENSIONS--
intl
--INI--
intl.default_locale=nl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->getMinimalDaysInFirstWeek());
var_dump(intlcal_get_minimal_days_in_first_week($intlcal));
?>
--EXPECT--
int(4)
int(4)
