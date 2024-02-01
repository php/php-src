--TEST--
IntlCalendar::set() basic test
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance();
var_dump($intlcal->set(IntlCalendar::FIELD_DAY_OF_MONTH, 2));
var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH));
var_dump(intlcal_set($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH, 3));
var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH));

?>
--EXPECTF--
bool(true)
int(2)

Deprecated: Function intlcal_set() is deprecated in %s on line %d
bool(true)
int(3)
