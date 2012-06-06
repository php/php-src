--TEST--
Bug #62017: datefmt_create with incorrectly encoded timezone leaks pattern
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set('intl.error_level', E_WARNING);
var_dump(
	datefmt_create('', IntlDateFormatter::NONE, IntlDateFormatter::NONE, "\xFF",
		IntlDateFormatter::GREGORIAN, 'a'));
var_dump(
	new IntlDateFormatter('', IntlDateFormatter::NONE, IntlDateFormatter::NONE, "Europe/Lisbon",
		IntlDateFormatter::GREGORIAN, "\x80"));
--EXPECTF--
Warning: datefmt_create(): datefmt_create: error converting timezone_str to UTF-16 in %s on line %d
NULL

Warning: IntlDateFormatter::__construct(): datefmt_create: error converting pattern to UTF-16 in %s on line %d
NULL

