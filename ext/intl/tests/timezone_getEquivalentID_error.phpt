--TEST--
IntlTimeZone::getEquivalentID(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getEquivalentID("foo\x80", 0));
?>
--EXPECTF--
Warning: IntlTimeZone::getEquivalentID(): intltz_get_equivalent_id: could not convert time zone id to UTF-16 in %s on line %d
bool(false)
