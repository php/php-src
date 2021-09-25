--TEST--
IntlTimeZone::getErrorMessage(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intltz_get_error_message(null));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: intltz_get_error_message(): Argument #1 ($timezone) must be of type IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_get_error_message(NULL)
#1 {main}
  thrown in %s on line %d
