--TEST--
IntlTimeZone::getCanonicalID(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getCanonicalID("foo\x81"));
?>
--EXPECTF--
Warning: IntlTimeZone::getCanonicalID(): intltz_get_canonical_id: could not convert time zone id to UTF-16 in %s on line %d
bool(false)
