--TEST--
IntlCalendar::isSet() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->isSet(IntlCalendar::FIELD_MINUTE));
$intlcal->clear(IntlCalendar::FIELD_MINUTE);
var_dump($intlcal->isSet(IntlCalendar::FIELD_MINUTE));
$intlcal->set(IntlCalendar::FIELD_MINUTE, 0);
var_dump(intlcal_is_set($intlcal, IntlCalendar::FIELD_MINUTE));
?>
==DONE==
--EXPECT--
bool(true)
bool(false)
bool(true)
==DONE==