--TEST--
IntlCalendar::before()/after(): bad arguments
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
	var_dump($c->after());
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->before());
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try {
	var_dump($c->after(1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->before(1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try{
	var_dump($c->after($c, 1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->before($c, 1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try {
	var_dump(intlcal_after($c));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_before($c));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
--EXPECT--
error: 0, IntlCalendar::after() expects exactly 1 parameter, 0 given

error: 0, IntlCalendar::before() expects exactly 1 parameter, 0 given

error: 0, IntlCalendar::after() expects parameter 1 to be IntlCalendar, int given

error: 0, IntlCalendar::before() expects parameter 1 to be IntlCalendar, int given

error: 0, IntlCalendar::after() expects exactly 1 parameter, 2 given

error: 0, IntlCalendar::before() expects exactly 1 parameter, 2 given

error: 0, intlcal_after() expects exactly 2 parameters, 1 given

error: 0, intlcal_before() expects exactly 2 parameters, 1 given
