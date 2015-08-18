--TEST--
IntlTimeZone::getDisplayName(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getDisplayName(array()));
var_dump($tz->getDisplayName(false, array()));
var_dump($tz->getDisplayName(false, -1));
var_dump($tz->getDisplayName(false, IntlTimeZone::DISPLAY_SHORT, array()));
var_dump($tz->getDisplayName(false, IntlTimeZone::DISPLAY_SHORT, NULL, NULL));

var_dump(intltz_get_display_name(null, IntlTimeZone::DISPLAY_SHORT, false, 'pt_PT'));

--EXPECTF--

Warning: IntlTimeZone::getDisplayName() expects parameter 1 to be boolean, array given in %s on line %d

Warning: IntlTimeZone::getDisplayName(): intltz_get_display_name: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getDisplayName() expects parameter 2 to be integer, array given in %s on line %d

Warning: IntlTimeZone::getDisplayName(): intltz_get_display_name: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getDisplayName(): intltz_get_display_name: wrong display type in %s on line %d
bool(false)

Warning: IntlTimeZone::getDisplayName() expects parameter 3 to be string, array given in %s on line %d

Warning: IntlTimeZone::getDisplayName(): intltz_get_display_name: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getDisplayName() expects at most 3 parameters, 4 given in %s on line %d

Warning: IntlTimeZone::getDisplayName(): intltz_get_display_name: bad arguments in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: Argument 1 passed to intltz_get_display_name() must be an instance of IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_get_display_name(NULL, 1, false, 'pt_PT')
#1 {main}
  thrown in %s on line %d
