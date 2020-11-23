--TEST--
IntlTimeZone::getID(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

intltz_get_id(null);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: intltz_get_id(): Argument #1 ($timezone) must be of type IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_get_id(NULL)
#1 {main}
  thrown in %s on line %d
