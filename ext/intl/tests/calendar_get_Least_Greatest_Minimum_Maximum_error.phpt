--TEST--
IntlCalendar::get/Least/Greatest/Minimum/Maximum(): bad arguments
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

var_dump($c->getLeastMaximum());
var_dump($c->getMaximum());
var_dump($c->getGreatestMinimum());
var_dump($c->getMinimum());

var_dump($c->getLeastMaximum(-1));
var_dump($c->getMaximum(-1));
var_dump($c->getGreatestMinimum(-1));
var_dump($c->getMinimum(-1));

var_dump(intlcal_get_least_maximum($c, -1));
var_dump(intlcal_get_maximum($c, -1));
var_dump(intlcal_get_greatest_minimum($c, -1));
var_dump(intlcal_get_minimum($c, -1));

function eh($errno, $errstr) {
echo "error: $errno, $errstr\n";
}
set_error_handler('eh');

try {
	var_dump(intlcal_get_least_maximum(1, 1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_maximum(1, 1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_greatest_minimum(1, -1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_minimum(1, -1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
--EXPECTF--
Warning: IntlCalendar::getLeastMaximum() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getLeastMaximum(): intlcal_get_least_maximum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getMaximum() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getMaximum(): intlcal_get_maximum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getGreatestMinimum() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getGreatestMinimum(): intlcal_get_greatest_minimum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getMinimum() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlCalendar::getMinimum(): intlcal_get_minimum: bad arguments in %s on line %d
bool(false)

Warning: IntlCalendar::getLeastMaximum(): intlcal_get_least_maximum: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::getMaximum(): intlcal_get_maximum: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::getGreatestMinimum(): intlcal_get_greatest_minimum: invalid field in %s on line %d
bool(false)

Warning: IntlCalendar::getMinimum(): intlcal_get_minimum: invalid field in %s on line %d
bool(false)

Warning: intlcal_get_least_maximum(): intlcal_get_least_maximum: invalid field in %s on line %d
bool(false)

Warning: intlcal_get_maximum(): intlcal_get_maximum: invalid field in %s on line %d
bool(false)

Warning: intlcal_get_greatest_minimum(): intlcal_get_greatest_minimum: invalid field in %s on line %d
bool(false)

Warning: intlcal_get_minimum(): intlcal_get_minimum: invalid field in %s on line %d
bool(false)
error: 0, Argument 1 passed to intlcal_get_least_maximum() must be an instance of IntlCalendar, integer given

error: 0, Argument 1 passed to intlcal_get_maximum() must be an instance of IntlCalendar, integer given

error: 0, Argument 1 passed to intlcal_get_greatest_minimum() must be an instance of IntlCalendar, integer given

error: 0, Argument 1 passed to intlcal_get_minimum() must be an instance of IntlCalendar, integer given
