--TEST--
IntlTimeZone::createTimeZone(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createTimeZone("foo\x80"));
?>
--EXPECTF--
Warning: IntlTimeZone::createTimeZone(): intltz_create_time_zone: could not convert time zone id to UTF-16 in %s on line %d
NULL
