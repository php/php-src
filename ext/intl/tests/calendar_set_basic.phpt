--TEST--
IntlCalendar::set() basic test
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

$intlcal = IntlCalendar::createInstance();
var_dump($intlcal->set(IntlCalendar::FIELD_DAY_OF_MONTH, 2));
var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH));
var_dump(intlcal_set($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH, 3));
var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH));

?>
==DONE==
--EXPECT--
bool(true)
int(2)
bool(true)
int(3)
==DONE==