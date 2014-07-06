--TEST--
IntlTimeZone::getCanonicalID(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getCanonicalID());
var_dump(IntlTimeZone::getCanonicalID(array()));
var_dump(IntlTimeZone::getCanonicalID("foo\x81"));
var_dump(IntlTimeZone::getCanonicalID('foobar', null));


--EXPECTF--

Warning: IntlTimeZone::getCanonicalID() expects at least 1 parameter, 0 given in %s on line %d

Warning: IntlTimeZone::getCanonicalID(): intltz_get_canonical_id: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getCanonicalID() expects parameter 1 to be string, array given in %s on line %d

Warning: IntlTimeZone::getCanonicalID(): intltz_get_canonical_id: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::getCanonicalID(): intltz_get_canonical_id: could not convert time zone id to UTF-16 in %s on line %d
bool(false)

Fatal error: Cannot pass parameter 2 by reference in %s on line %d
