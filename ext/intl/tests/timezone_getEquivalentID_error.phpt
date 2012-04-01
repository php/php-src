--TEST--
IntlTimeZone::getEquivalentID(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getEquivalentID('foo'));
var_dump(IntlTimeZone::getEquivalentID('foo', 'bar'));
var_dump(IntlTimeZone::getEquivalentID('Europe/Lisbon', 0, 1));
var_dump(IntlTimeZone::getEquivalentID("foo\x80", 0));

--EXPECTF--

Warning: IntlTimeZone::getEquivalentID() expects exactly 2 parameters, 1 given in %s on line %d

Warning: IntlTimeZone::getEquivalentID(): intltz_get_equivalent_id: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getEquivalentID() expects parameter 2 to be long, string given in %s on line %d

Warning: IntlTimeZone::getEquivalentID(): intltz_get_equivalent_id: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getEquivalentID() expects exactly 2 parameters, 3 given in %s on line %d

Warning: IntlTimeZone::getEquivalentID(): intltz_get_equivalent_id: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getEquivalentID(): intltz_get_equivalent_id: could not convert time zone id to UTF-16 in %s on line %d
bool(false)
