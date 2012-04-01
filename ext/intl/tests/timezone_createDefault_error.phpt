--TEST--
IntlTimeZone::createDefault(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createDefault(4));


--EXPECTF--

Warning: IntlTimeZone::createDefault() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::createDefault(): intltz_create_default: bad arguments in %s on line %d
NULL
