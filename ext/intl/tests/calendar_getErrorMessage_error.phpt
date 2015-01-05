--TEST--
IntlCalendar::getErrorMessage(): bad arguments
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

var_dump($c->getErrorMessage(array()));

var_dump(intlcal_get_error_message(null));

--EXPECTF--

Warning: IntlCalendar::getErrorMessage() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlCalendar::getErrorMessage(): intlcal_get_error_message: bad arguments in %s on line %d
bool(false)

Catchable fatal error: Argument 1 passed to intlcal_get_error_message() must be an instance of IntlCalendar, null given in %s on line %d
