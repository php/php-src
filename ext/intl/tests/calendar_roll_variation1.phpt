--TEST--
IntlCalendar::roll() bool argument variation
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = new IntlGregorianCalendar(2012, 1, 28);
var_dump($intlcal->roll(IntlCalendar::FIELD_DAY_OF_MONTH, true));
var_dump($intlcal->get(IntlCalendar::FIELD_MONTH)); //1 (Feb)
var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH)); //29

var_dump(intlcal_roll($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH, false));
var_dump($intlcal->get(IntlCalendar::FIELD_MONTH)); //1 (Feb)
var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH)); //28

?>
--EXPECTF--
Deprecated: IntlCalendar::roll(): Passing bool is deprecated, use 1 or -1 instead in %s on line %d
bool(true)
int(1)
int(29)

Deprecated: intlcal_roll(): Passing bool is deprecated, use 1 or -1 instead in %s on line %d
bool(true)
int(1)
int(28)
