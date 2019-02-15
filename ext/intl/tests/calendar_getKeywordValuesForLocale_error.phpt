--TEST--
IntlCalendar::getKeywordValuesForLocale(): bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlcal_get_keyword_values_for_locale(1, 2));
var_dump(IntlCalendar::getKeywordValuesForLocale(1, 2, array()));
--EXPECTF--
Warning: intlcal_get_keyword_values_for_locale() expects exactly 3 parameters, 2 given in %s on line %d

Warning: intlcal_get_keyword_values_for_locale(): intlcal_get_keyword_values_for_locale: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getKeywordValuesForLocale() expects parameter 3 to be bool, array given in %s on line %d

Warning: IntlCalendar::getKeywordValuesForLocale(): intlcal_get_keyword_values_for_locale: bad arguments in %s on line %d
bool(false)
