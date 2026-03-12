--TEST--
IntlCalendar::setMinimalDaysInFirstWeek() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
var_dump(
        $intlcal->setMinimalDaysInFirstWeek(6),
        $intlcal->getMinimalDaysInFirstWeek(),
        intlcal_set_minimal_days_in_first_week($intlcal, 5),
        $intlcal->getMinimalDaysInFirstWeek()
);
?>
--EXPECT--
bool(true)
int(6)
bool(true)
int(5)
