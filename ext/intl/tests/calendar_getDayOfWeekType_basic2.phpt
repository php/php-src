--TEST--
IntlCalendar::getDayOfWeekType() basic test
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->setTime(strtotime('2012-02-29 00:00:00 +0000') * 1000);
var_dump(
        intlcal_get_day_of_week_type($intlcal, IntlCalendar::DOW_SUNDAY),
        $intlcal->getDayOfWeekType(IntlCalendar::DOW_MONDAY),
        $intlcal->getDayOfWeekType(IntlCalendar::DOW_TUESDAY),
        $intlcal->getDayOfWeekType(IntlCalendar::DOW_FRIDAY),
        $intlcal->getDayOfWeekType(IntlCalendar::DOW_SATURDAY)
);

?>
--EXPECT--
int(1)
int(0)
int(0)
int(0)
int(1)
