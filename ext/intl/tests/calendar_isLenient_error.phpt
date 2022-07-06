--TEST--
IntlCalendar::isLenient(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlcal_is_lenient(1));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: intlcal_is_lenient(): Argument #1 ($calendar) must be of type IntlCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlcal_is_lenient(1)
#1 {main}
  thrown in %s on line %d
