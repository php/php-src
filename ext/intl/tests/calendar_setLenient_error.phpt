--TEST--
IntlCalendar::setLenient(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlcal_set_lenient(1, false));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: intlcal_set_lenient(): Argument #1 ($calendar) must be of type IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_set_lenient(1, false)
#1 {main}
  thrown in %s on line %d
