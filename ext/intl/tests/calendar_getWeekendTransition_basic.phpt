--TEST--
IntlCalendar::getWeekendTransition() basic test
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance();
var_dump($intlcal->getWeekendTransition(IntlCalendar::DOW_SUNDAY));
var_dump(intlcal_get_weekend_transition($intlcal, IntlCalendar::DOW_SUNDAY));
?>
--EXPECT--
int(86400000)
int(86400000)
