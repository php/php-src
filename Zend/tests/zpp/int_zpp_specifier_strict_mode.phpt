--TEST--
Test int ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_int($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_int_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_int_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_int_or_null_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	$ref =& $type;
	try {
		var_dump(zend_int($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_int_or_null($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_int_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_int_or_null_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECT--
Using null:
TypeError: zend_int(): Argument #1 ($param) must be of type int, null given
NULL
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, null given
NULL
TypeError: zend_int(): Argument #1 ($param) must be of type int, null given
NULL
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, null given
NULL
Using false:
TypeError: zend_int(): Argument #1 ($param) must be of type int, false given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, false given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, false given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, false given
TypeError: zend_int(): Argument #1 ($param) must be of type int, false given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, false given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, false given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, false given
Using true:
TypeError: zend_int(): Argument #1 ($param) must be of type int, true given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, true given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, true given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, true given
TypeError: zend_int(): Argument #1 ($param) must be of type int, true given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, true given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, true given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, true given
Using 42:
int(42)
int(42)
int(42)
int(42)
int(42)
int(42)
int(42)
int(42)
Using 73.5:
TypeError: zend_int(): Argument #1 ($param) must be of type int, float given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, float given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, float given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, float given
TypeError: zend_int(): Argument #1 ($param) must be of type int, float given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, float given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, float given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, float given
Using 'string':
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
Using '15':
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
Using '56.7':
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
Using 'stdClass':
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
Using anon class name:
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, string given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, string given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, string given
Using []:
TypeError: zend_int(): Argument #1 ($param) must be of type int, array given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, array given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, array given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, array given
TypeError: zend_int(): Argument #1 ($param) must be of type int, array given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, array given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, array given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, array given
Using new stdClass():
TypeError: zend_int(): Argument #1 ($param) must be of type int, stdClass given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, stdClass given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, stdClass given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, stdClass given
TypeError: zend_int(): Argument #1 ($param) must be of type int, stdClass given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, stdClass given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, stdClass given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, stdClass given
Using new S():
TypeError: zend_int(): Argument #1 ($param) must be of type int, S given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, S given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, S given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, S given
TypeError: zend_int(): Argument #1 ($param) must be of type int, S given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, S given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, S given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, S given
Using STDOUT:
TypeError: zend_int(): Argument #1 ($param) must be of type int, resource given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, resource given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, resource given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, resource given
TypeError: zend_int(): Argument #1 ($param) must be of type int, resource given
TypeError: zend_int_or_null(): Argument #1 ($param) must be of type ?int, resource given
TypeError: zend_int_slow_zpp(): Argument #1 ($param) must be of type int, resource given
TypeError: zend_int_or_null_slow_zpp(): Argument #1 ($param) must be of type ?int, resource given
