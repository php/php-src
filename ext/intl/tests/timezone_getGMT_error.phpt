--TEST--
IntlTimeZone::getGMT(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getGMT(4));
--EXPECTF--
Warning: IntlTimeZone::getGMT() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getGMT(): intltz_get_gmt: bad arguments in %s on line %d
NULL
