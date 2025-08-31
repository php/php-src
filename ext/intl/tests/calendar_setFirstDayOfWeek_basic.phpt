--TEST--
IntlCalendar::setFirstDayOfWeek() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
var_dump(
        IntlCalendar::DOW_TUESDAY,
        $intlcal->setFirstDayOfWeek(IntlCalendar::DOW_TUESDAY),
        $intlcal->getFirstDayOfWeek(),
        intlcal_set_first_day_of_week($intlcal, IntlCalendar::DOW_WEDNESDAY),
        $intlcal->getFirstDayOfWeek()
);
?>
--EXPECT--
int(3)
bool(true)
int(3)
bool(true)
int(4)
