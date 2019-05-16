--TEST--
IntlCalendar::getMinimalDaysInFirstWeek() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->getMinimalDaysInFirstWeek());
var_dump(intlcal_get_minimal_days_in_first_week($intlcal));
?>
==DONE==
--EXPECT--
int(4)
int(4)
==DONE==
