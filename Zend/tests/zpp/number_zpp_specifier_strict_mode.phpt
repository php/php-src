--TEST--
Test number ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_number($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_number_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_number_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_number_or_null_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	$ref =& $type;
	try {
		var_dump(zend_number($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_number_or_null($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_number_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_number_or_null_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECT--
Using null:
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, null given
NULL
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, null given
NULL
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, null given
NULL
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, null given
NULL
Using false:
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, false given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, false given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, false given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, false given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, false given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, false given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, false given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, false given
Using true:
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, true given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, true given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, true given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, true given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, true given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, true given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, true given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, true given
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
float(73.5)
float(73.5)
float(73.5)
float(73.5)
float(73.5)
float(73.5)
float(73.5)
float(73.5)
Using 'string':
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
Using '15':
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
Using '56.7':
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
Using 'stdClass':
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
Using anon class name:
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, string given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, string given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, string given
Using []:
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, array given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, array given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, array given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, array given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, array given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, array given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, array given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, array given
Using new stdClass():
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, stdClass given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, stdClass given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, stdClass given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, stdClass given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, stdClass given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, stdClass given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, stdClass given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, stdClass given
Using new S():
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, S given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, S given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, S given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, S given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, S given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, S given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, S given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, S given
Using STDOUT:
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, resource given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, resource given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, resource given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, resource given
TypeError: zend_number(): Argument #1 ($param) must be of type int|float, resource given
TypeError: zend_number_or_null(): Argument #1 ($param) must be of type int|float|null, resource given
TypeError: zend_number_slow_zpp(): Argument #1 ($param) must be of type int|float, resource given
TypeError: zend_number_or_null_slow_zpp(): Argument #1 ($param) must be of type int|float|null, resource given
