--TEST--
IntlTimeZone::useDaylightTime(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

intltz_use_daylight_time(null);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: intltz_use_daylight_time(): Argument #1 ($timezone) must be of type IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_use_daylight_time(NULL)
#1 {main}
  thrown in %s on line %d
