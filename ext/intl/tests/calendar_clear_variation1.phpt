--TEST--
IntlCalendar::clear() 1 arg variation
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
//print_R($intlcal);
var_dump($intlcal->isSet(IntlCalendar::FIELD_MONTH));
var_dump($intlcal->clear(IntlCalendar::FIELD_MONTH));
var_dump($intlcal->isSet(IntlCalendar::FIELD_MONTH));
var_dump($intlcal->get(IntlCalendar::FIELD_MONTH));
var_dump($intlcal->getTime());
var_dump($intlcal->set(IntlCalendar::FIELD_MONTH, 0));
var_dump($intlcal->getTime());
//print_R($intlcal);
var_dump(
    $intlcal->getTime(),
    strtotime('2012-01-29 05:06:07 +0000') * 1000.
);
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
int(1)
float(1327813567000)
bool(true)
float(1327813567000)
float(1327813567000)
float(1327813567000)