--TEST--
IntlCalendar::isEquivalentTo(): bad arguments
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

function eh($errno, $errstr) {
echo "error: $errno, $errstr\n";
}
set_error_handler('eh');

try {
	var_dump($c->isEquivalentTo(0));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->isEquivalentTo($c, 1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->isEquivalentTo(1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try {
	var_dump(intlcal_is_equivalent_to($c));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_is_equivalent_to($c, 1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_is_equivalent_to(1, $c));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
--EXPECT--
error: 0, Argument 1 passed to IntlCalendar::isEquivalentTo() must be an instance of IntlCalendar, int given

error: 2, IntlCalendar::isEquivalentTo() expects exactly 1 parameter, 2 given
error: 2, IntlCalendar::isEquivalentTo(): intlcal_is_equivalent_to: bad arguments
bool(false)
error: 0, Argument 1 passed to IntlCalendar::isEquivalentTo() must be an instance of IntlCalendar, int given

error: 2, intlcal_is_equivalent_to() expects exactly 2 parameters, 1 given
error: 2, intlcal_is_equivalent_to(): intlcal_is_equivalent_to: bad arguments
bool(false)
error: 0, Argument 2 passed to intlcal_is_equivalent_to() must be an instance of IntlCalendar, int given

error: 0, Argument 1 passed to intlcal_is_equivalent_to() must be an instance of IntlCalendar, int given
