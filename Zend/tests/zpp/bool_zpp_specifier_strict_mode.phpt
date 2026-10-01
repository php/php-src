--TEST--
Test bool ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_bool($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_bool_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_bool_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_bool_or_null_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	$ref =& $type;
	try {
		var_dump(zend_bool($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_bool_or_null($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_bool_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_bool_or_null_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECT--
Using null:
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, null given
NULL
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, null given
NULL
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, null given
NULL
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, null given
NULL
Using false:
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Using true:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Using 42:
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, int given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, int given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, int given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, int given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, int given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, int given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, int given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, int given
Using 73.5:
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, float given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, float given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, float given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, float given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, float given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, float given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, float given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, float given
Using 'string':
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
Using '15':
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
Using '56.7':
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
Using 'stdClass':
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
Using anon class name:
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, string given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, string given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, string given
Using []:
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, array given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, array given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, array given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, array given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, array given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, array given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, array given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, array given
Using new stdClass():
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, stdClass given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, stdClass given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, stdClass given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, stdClass given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, stdClass given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, stdClass given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, stdClass given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, stdClass given
Using new S():
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, S given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, S given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, S given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, S given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, S given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, S given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, S given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, S given
Using STDOUT:
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, resource given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, resource given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, resource given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, resource given
TypeError: zend_bool(): Argument #1 ($param) must be of type bool, resource given
TypeError: zend_bool_or_null(): Argument #1 ($param) must be of type ?bool, resource given
TypeError: zend_bool_slow_zpp(): Argument #1 ($param) must be of type bool, resource given
TypeError: zend_bool_or_null_slow_zpp(): Argument #1 ($param) must be of type ?bool, resource given
