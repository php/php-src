--TEST--
IntlCalendar::getMinimum(), ::getActualMinimum(), ::getGreatestMinimum() basic test
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->setTime(strtotime('2012-02-29 05:06:07 +0000') * 1000);
var_dump(
        $intlcal->getGreatestMinimum(IntlCalendar::FIELD_DAY_OF_MONTH),
        intlcal_get_greatest_minimum($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH),
        $intlcal->getActualMinimum(IntlCalendar::FIELD_DAY_OF_MONTH),
        intlcal_get_actual_minimum($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH),
        $intlcal->getMinimum(IntlCalendar::FIELD_DAY_OF_MONTH),
        intlcal_get_minimum($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH)
);

?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)