--TEST--
IntlCalendar::get() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
$intlcal->set(IntlCalendar::FIELD_DAY_OF_MONTH, 4);

var_dump($intlcal->get(IntlCalendar::FIELD_DAY_OF_MONTH));
var_dump(intlcal_get($intlcal, IntlCalendar::FIELD_DAY_OF_MONTH));

?>
==DONE==
--EXPECT--
int(4)
int(4)
==DONE==