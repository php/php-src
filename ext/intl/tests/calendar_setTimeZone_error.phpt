--TEST--
IntlCalendar::setTimeZone(): bad arguments
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

var_dump($c->setTimeZone($gmt, 2));
var_dump($c->setTimeZone(1));
var_dump($c->setTimeZone());

var_dump(intlcal_set_time_zone($c, 1));
var_dump(intlcal_set_time_zone(1, $gmt));
--EXPECT--
error: 2, IntlCalendar::setTimeZone() expects exactly 1 parameter, 2 given
error: 2, IntlCalendar::setTimeZone(): intlcal_set_time_zone: bad arguments
bool(false)
error: 4096, Argument 1 passed to IntlCalendar::setTimeZone() must be an instance of IntlTimeZone, integer given
error: 2, IntlCalendar::setTimeZone() expects parameter 1 to be IntlTimeZone, integer given
error: 2, IntlCalendar::setTimeZone(): intlcal_set_time_zone: bad arguments
bool(false)
error: 2, IntlCalendar::setTimeZone() expects exactly 1 parameter, 0 given
error: 2, IntlCalendar::setTimeZone(): intlcal_set_time_zone: bad arguments
bool(false)
error: 4096, Argument 2 passed to intlcal_set_time_zone() must be an instance of IntlTimeZone, integer given
error: 2, intlcal_set_time_zone() expects parameter 2 to be IntlTimeZone, integer given
error: 2, intlcal_set_time_zone(): intlcal_set_time_zone: bad arguments
bool(false)
error: 4096, Argument 1 passed to intlcal_set_time_zone() must be an instance of IntlCalendar, integer given
error: 2, intlcal_set_time_zone() expects parameter 1 to be IntlCalendar, integer given
error: 2, intlcal_set_time_zone(): intlcal_set_time_zone: bad arguments
bool(false)
