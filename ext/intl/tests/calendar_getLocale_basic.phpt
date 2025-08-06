--TEST--
IntlCalendar::getLocale() basic test
--EXTENSIONS--
intl
--INI--
intl.default_locale=nl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('UTC');
var_dump($intlcal->getLocale(Locale::ACTUAL_LOCALE));
var_dump(intlcal_get_locale($intlcal, Locale::VALID_LOCALE));
?>
--EXPECT--
string(2) "nl"
string(5) "nl_NL"
