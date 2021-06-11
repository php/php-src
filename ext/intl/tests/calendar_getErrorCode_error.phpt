--TEST--
IntlCalendar::getErrorCode(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlcal_get_error_code(null));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: intlcal_get_error_code(): Argument #1 ($calendar) must be of type IntlCalendar, null given in %s:%d
Stack trace:
#0 %s(%d): intlcal_get_error_code(NULL)
#1 {main}
  thrown in %s on line %d
