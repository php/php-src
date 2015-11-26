--TEST--
IntlTimeZone::getTZDataVersion(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getTZDataVersion('foo'));

--EXPECTF--

Warning: IntlTimeZone::getTZDataVersion() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getTZDataVersion(): intltz_get_tz_data_version: bad arguments in %s on line %d
bool(false)
