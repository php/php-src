--TEST--
IntlTimeZone::countEquivalentIDs(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::countEquivalentIDs("foo\x80"));
?>
--EXPECTF--
Warning: IntlTimeZone::countEquivalentIDs(): intltz_count_equivalent_ids: could not convert time zone id to UTF-16 in %s on line %d
bool(false)
