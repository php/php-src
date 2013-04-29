--TEST--
IntlCalendar::getAvailableLocales(): bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlcal_get_available_locales(1));
var_dump(IntlCalendar::getAvailableLocales(2));

--EXPECTF--

Warning: intlcal_get_available_locales() expects exactly 0 parameters, 1 given in %s on line %d

Warning: intlcal_get_available_locales(): intlcal_get_available_locales: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getAvailableLocales() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::getAvailableLocales(): intlcal_get_available_locales: bad arguments in %s on line %d
bool(false)
