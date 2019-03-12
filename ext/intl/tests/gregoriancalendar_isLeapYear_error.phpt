--TEST--
IntlGregorianCalendar::isLeapYear(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlgregcal_is_leap_year(1, 2));

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to intlgregcal_is_leap_year() must be an instance of IntlGregorianCalendar, int given in %s:%d
Stack trace:
#0 %s(%d): intlgregcal_is_leap_year(1, 2)
#1 {main}
  thrown in %s on line %d
