--TEST--
IntlCalendar::roll() basic test
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = new IntlGregorianCalendar(2012, 1, 28);
var_dump($intlcal->roll(IntlCalendar::FIELD_DAY_OF_MONTH, 2));
var_dump($intlcal->get(IntlCalendar::FIELD_MONTH)); //1 (Feb)
var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH)); //1

$intlcal = new IntlGregorianCalendar(2012, 1, 28);
var_dump(intlcal_roll($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH, 2));
var_dump($intlcal->get(IntlCalendar::FIELD_MONTH)); //1 (Feb)
var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH)); //1


?>
--EXPECT--
bool(true)
int(1)
int(1)
bool(true)
int(1)
int(1)