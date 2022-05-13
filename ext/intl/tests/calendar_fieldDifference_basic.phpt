--TEST--
IntlCalendar::fieldDifference() basic test
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
        $intlcal->fieldDifference(
                strtotime('2012-02-29 06:06:08 +0000') * 1000,
                IntlCalendar::FIELD_SECOND),
        $intlcal->get(IntlCalendar::FIELD_HOUR_OF_DAY));


$intlcal->setTime(strtotime('2012-02-29 05:06:07 +0000') * 1000);
var_dump(
        intlcal_field_difference(
                $intlcal,
                strtotime('2012-02-29 06:07:08 +0000') * 1000,
                IntlCalendar::FIELD_MINUTE));
?>
--EXPECT--
int(3601)
int(6)
int(61)