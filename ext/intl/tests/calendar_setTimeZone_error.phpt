--TEST--
IntlCalendar::setTimeZone(): bad arguments
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

$gmt = IntlTimeZone::getGMT();

function eh($errno, $errstr) {
echo "error: $errno, $errstr\n";
}
set_error_handler('eh');

try {
	var_dump($c->setTimeZone($gmt, 2));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump($c->setTimeZone());
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try{
	var_dump(intlcal_set_time_zone($c, 1, 2));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try{
	var_dump(intlcal_set_time_zone(1, $gmt));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
--EXPECT--
error: 2, IntlCalendar::setTimeZone() expects exactly 1 parameter, 2 given
error: 2, IntlCalendar::setTimeZone(): intlcal_set_time_zone: bad arguments
bool(false)
error: 2, IntlCalendar::setTimeZone() expects exactly 1 parameter, 0 given
error: 2, IntlCalendar::setTimeZone(): intlcal_set_time_zone: bad arguments
bool(false)
error: 2, intlcal_set_time_zone() expects exactly 2 parameters, 3 given
error: 2, intlcal_set_time_zone(): intlcal_set_time_zone: bad arguments
bool(false)
error: 0, Argument 1 passed to intlcal_set_time_zone() must be an instance of IntlCalendar, int given
