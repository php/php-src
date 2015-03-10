--TEST--
IntlCalendar::clear(): bad arguments
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

var_dump($c->clear(1, 2));
var_dump($c->clear(-1));

var_dump(intlcal_clear($c, -1));
var_dump(intlcal_clear(1, 2));
--EXPECTF--

Warning: IntlCalendar::clear(): intlcal_clear: too many arguments in %s on line %d
bool(false)

Warning: IntlCalendar::clear(): intlcal_clear: invalid field in %s on line %d
bool(false)

Warning: intlcal_clear(): intlcal_clear: invalid field in %s on line %d
bool(false)

Fatal error: Argument 1 passed to intlcal_clear() must be an instance of IntlCalendar, integer given in %s on line %d
