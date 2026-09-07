--TEST--
Test number ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

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
--EXPECTF--
Using null:

Deprecated: zend_number(): Passing null to parameter #1 ($param) of type int|float is deprecated in %s on line %d
int(0)
NULL

Deprecated: zend_number_slow_zpp(): Passing null to parameter #1 ($param) of type int|float is deprecated in %s on line %d
int(0)
NULL

Deprecated: zend_number(): Passing null to parameter #1 ($param) of type int|float is deprecated in %s on line %d
int(0)
NULL

Deprecated: zend_number_slow_zpp(): Passing null to parameter #1 ($param) of type int|float is deprecated in %s on line %d
int(0)
NULL
Using false:
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
Using true:
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
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
int(15)
int(15)
int(15)
int(15)
int(15)
int(15)
int(15)
int(15)
Using '56.7':
float(56.7)
float(56.7)
float(56.7)
float(56.7)
float(56.7)
float(56.7)
float(56.7)
float(56.7)
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
