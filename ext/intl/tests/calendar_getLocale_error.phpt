--TEST--
IntlCalendar::getLocale(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

var_dump($c->getLocale());
var_dump($c->getLocale(2));
var_dump($c->getLocale(2, 3));

var_dump(intlcal_get_locale($c));
var_dump(intlcal_get_locale(1));

--EXPECTF--

Warning: IntlCalendar::getLocale() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getLocale(): intlcal_get_locale: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getLocale(): intlcal_get_locale: invalid locale type in %s on line %d
bool(false)

Warning: IntlCalendar::getLocale() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlCalendar::getLocale(): intlcal_get_locale: bad arguments in %s on line %d
bool(false)

Warning: intlcal_get_locale() expects exactly 2 parameters, 1 given in %s on line %d

Warning: intlcal_get_locale(): intlcal_get_locale: bad arguments in %s on line %d
bool(false)

Catchable fatal error: Argument 1 passed to intlcal_get_locale() must be an instance of IntlCalendar, integer given in %s on line %d
