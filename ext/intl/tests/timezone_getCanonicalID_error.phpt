--TEST--
IntlTimeZone::getCanonicalID(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::getCanonicalID("foo\x81"));
?>
--EXPECTF--
Warning: IntlTimeZone::getCanonicalID(): could not convert time zone id to UTF-16 in %s on line %d
bool(false)
