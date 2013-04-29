--TEST--
IntlTimeZone::createTimeZone(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlTimeZone::createTimeZone());
var_dump(IntlTimeZone::createTimeZone(new stdClass));
var_dump(IntlTimeZone::createTimeZone("foo bar", 4));
var_dump(IntlTimeZone::createTimeZone("foo\x80"));

--EXPECTF--

Warning: IntlTimeZone::createTimeZone() expects exactly 1 parameter, 0 given in %s on line %d

Warning: IntlTimeZone::createTimeZone(): intltz_create_time_zone: bad arguments in %s on line %d
NULL

Warning: IntlTimeZone::createTimeZone() expects parameter 1 to be string, object given in %s on line %d

Warning: IntlTimeZone::createTimeZone(): intltz_create_time_zone: bad arguments in %s on line %d
NULL

Warning: IntlTimeZone::createTimeZone() expects exactly 1 parameter, 2 given in %s on line %d

Warning: IntlTimeZone::createTimeZone(): intltz_create_time_zone: bad arguments in %s on line %d
NULL

Warning: IntlTimeZone::createTimeZone(): intltz_create_time_zone: could not convert time zone id to UTF-16 in %s on line %d
NULL
