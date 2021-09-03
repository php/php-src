--TEST--
IntlCalendar::getDayOfWeekType() basic test
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (version_compare(INTL_ICU_VERSION, '52.1') >= 0)
    die('skip for ICU < 52.1');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

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
int(3)
int(0)
int(0)
int(0)
int(1)
