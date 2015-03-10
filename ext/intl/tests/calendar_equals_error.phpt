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
} catch (EngineException $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->equals(new stdclass));
} catch (EngineException $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->equals(1, 2));
} catch (EngineException $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}


try {
	var_dump(intlcal_equals($c, array()));
} catch (EngineException $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_equals(1, $c));
} catch (EngineException $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

--EXPECT--
error: 2, IntlCalendar::equals() expects exactly 1 parameter, 0 given
error: 2, IntlCalendar::equals(): intlcal_equals: bad arguments
bool(false)
error: 1, Argument 1 passed to IntlCalendar::equals() must be an instance of IntlCalendar, instance of stdClass given

error: 1, Argument 1 passed to IntlCalendar::equals() must be an instance of IntlCalendar, integer given

error: 1, Argument 2 passed to intlcal_equals() must be an instance of IntlCalendar, array given

error: 1, Argument 1 passed to intlcal_equals() must be an instance of IntlCalendar, integer given
