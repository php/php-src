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

var_dump($c->equals());
var_dump($c->equals(new stdclass));
var_dump($c->equals(1, 2));

var_dump(intlcal_equals($c, array()));
var_dump(intlcal_equals(1, $c));

--EXPECT--
error: 2, IntlCalendar::equals() expects exactly 1 parameter, 0 given
error: 2, IntlCalendar::equals(): intlcal_equals: bad arguments
bool(false)
error: 4096, Argument 1 passed to IntlCalendar::equals() must be an instance of IntlCalendar, instance of stdClass given
error: 2, IntlCalendar::equals() expects parameter 1 to be IntlCalendar, object given
error: 2, IntlCalendar::equals(): intlcal_equals: bad arguments
bool(false)
error: 4096, Argument 1 passed to IntlCalendar::equals() must be an instance of IntlCalendar, integer given
error: 2, IntlCalendar::equals() expects exactly 1 parameter, 2 given
error: 2, IntlCalendar::equals(): intlcal_equals: bad arguments
bool(false)
error: 4096, Argument 2 passed to intlcal_equals() must be an instance of IntlCalendar, array given
error: 2, intlcal_equals() expects parameter 2 to be IntlCalendar, array given
error: 2, intlcal_equals(): intlcal_equals: bad arguments
bool(false)
error: 4096, Argument 1 passed to intlcal_equals() must be an instance of IntlCalendar, integer given
error: 2, intlcal_equals() expects parameter 1 to be IntlCalendar, integer given
error: 2, intlcal_equals(): intlcal_equals: bad arguments
bool(false)
