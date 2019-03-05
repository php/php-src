--TEST--
IntlTimeZone::getDSTSavings(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intltz_get_dst_savings(null));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to intltz_get_dst_savings() must be an instance of IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_get_dst_savings(NULL)
#1 {main}
  thrown in %s on line %d
