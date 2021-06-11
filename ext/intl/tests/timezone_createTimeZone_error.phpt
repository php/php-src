--TEST--
IntlTimeZone::createTimeZone(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createTimeZone("foo\x80"));
?>
--EXPECTF--
Warning: IntlTimeZone::createTimeZone(): intltz_create_time_zone: could not convert time zone id to UTF-16 in %s on line %d
NULL
