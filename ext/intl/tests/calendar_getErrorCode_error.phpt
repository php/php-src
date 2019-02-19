--TEST--
IntlCalendar::getErrorCode(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlcal_get_error_code(null));
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to intlcal_get_error_code() must be an instance of IntlCalendar, null given in %s:%d
Stack trace:
#0 %s(%d): intlcal_get_error_code(NULL)
#1 {main}
  thrown in %s on line %d
