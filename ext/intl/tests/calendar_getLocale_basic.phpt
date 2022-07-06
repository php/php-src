--TEST--
IntlCalendar::getLocale() basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "nl");

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->getLocale(Locale::ACTUAL_LOCALE));
var_dump(intlcal_get_locale($intlcal, Locale::VALID_LOCALE));
?>
--EXPECT--
string(2) "nl"
string(5) "nl_NL"
