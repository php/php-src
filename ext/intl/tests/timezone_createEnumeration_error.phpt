--TEST--
IntlTimeZone::createEnumeration(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createEnumeration(array()));
var_dump(IntlTimeZone::createEnumeration(1, 2));
--EXPECTF--
Warning: IntlTimeZone::createEnumeration(): intltz_create_enumeration: invalid argument type in %s on line %d
bool(false)

Warning: IntlTimeZone::createEnumeration() expects at most 1 parameter, 2 given in %s on line %d

Warning: IntlTimeZone::createEnumeration(): intltz_create_enumeration: bad arguments in %s on line %d
bool(false)
