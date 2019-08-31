--TEST--
IntlCalendar::equals(): bad arguments
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
	var_dump($c->equals());
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->equals(new stdclass));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->equals(1, 2));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}


try {
	var_dump(intlcal_equals($c, array()));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_equals(1, $c));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
--EXPECT--
error: 0, IntlCalendar::equals() expects exactly 1 parameter, 0 given

error: 0, IntlCalendar::equals() expects parameter 1 to be IntlCalendar, object given

error: 0, IntlCalendar::equals() expects exactly 1 parameter, 2 given

error: 0, intlcal_equals() expects parameter 2 to be IntlCalendar, array given

error: 0, intlcal_equals() expects parameter 1 to be IntlCalendar, int given
