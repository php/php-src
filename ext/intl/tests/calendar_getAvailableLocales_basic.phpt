--TEST--
IntlCalendar::getAvailableLocales() basic test
--EXTENSIONS--
intl
--INI--
intl.default_locale=nl
--FILE--
<?php

$locales = IntlCalendar::getAvailableLocales();
var_dump(count($locales) > 100);

$locales = intlcal_get_available_locales();
var_dump(in_array('pt', $locales));

?>
--EXPECT--
bool(true)
bool(true)
