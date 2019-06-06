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
error: 0, intlcal_get_least_maximum() expects parameter 1 to be IntlCalendar, int given

error: 0, intlcal_get_maximum() expects parameter 1 to be IntlCalendar, int given

error: 0, intlcal_get_greatest_minimum() expects parameter 1 to be IntlCalendar, int given

error: 0, intlcal_get_minimum() expects parameter 1 to be IntlCalendar, int given
