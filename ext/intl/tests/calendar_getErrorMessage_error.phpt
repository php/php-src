--TEST--
IntlCalendar::getErrorMessage(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlcal_get_error_message(null));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: intlcal_get_error_message(): Argument #1 ($calendar) must be of type IntlCalendar, null given in %s:%d
Stack trace:
#0 %s(%d): intlcal_get_error_message(NULL)
#1 {main}
  thrown in %s on line %d
