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
try {
	var_dump(
		new IntlDateFormatter('', IntlDateFormatter::NONE, IntlDateFormatter::NONE, "Europe/Lisbon",
			IntlDateFormatter::GREGORIAN, "\x80"));
} catch (IntlException $e) {
	echo PHP_EOL."Exception: " . $e->getMessage() . " in " . $e->getFile() . " on line " . $e->getLine() . PHP_EOL;
}
--EXPECTF--
Warning: datefmt_create(): datefmt_create: Time zone identifier given is not a valid UTF-8 string in %s on line %d
NULL

Exception: IntlDateFormatter::__construct(): datefmt_create: error converting pattern to UTF-16 in %sbug62017.php on line %d
