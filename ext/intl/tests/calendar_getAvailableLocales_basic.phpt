--TEST--
IntlCalendar::getAvailableLocales() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$locales = IntlCalendar::getAvailableLocales();
var_dump(count($locales) > 100);

$locales = intlcal_get_available_locales();
var_dump(in_array('pt', $locales));

?>
==DONE==
--EXPECT--
bool(true)
bool(true)
==DONE==