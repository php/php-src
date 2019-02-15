--TEST--
IntlTimeZone::countEquivalentIDs(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::countEquivalentIDs());
var_dump(IntlTimeZone::countEquivalentIDs(array()));
var_dump(IntlTimeZone::countEquivalentIDs("foo\x80"));
var_dump(IntlTimeZone::countEquivalentIDs("foo bar", 7));
--EXPECTF--
Warning: IntlTimeZone::countEquivalentIDs() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlTimeZone::countEquivalentIDs(): intltz_count_equivalent_ids: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::countEquivalentIDs() expects parameter 1 to be string, array given in %s on line %d

Warning: IntlTimeZone::countEquivalentIDs(): intltz_count_equivalent_ids: bad arguments in %s on line %d
bool(false)

Warning: IntlTimeZone::countEquivalentIDs(): intltz_count_equivalent_ids: could not convert time zone id to UTF-16 in %s on line %d
bool(false)

Warning: IntlTimeZone::countEquivalentIDs() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlTimeZone::countEquivalentIDs(): intltz_count_equivalent_ids: bad arguments in %s on line %d
bool(false)
