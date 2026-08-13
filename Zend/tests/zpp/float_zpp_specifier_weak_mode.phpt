--TEST--
Test float ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_float($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_float_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_float_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_float_or_null_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	$ref =& $type;
	try {
		var_dump(zend_float($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_float_or_null($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_float_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_float_or_null_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:

Deprecated: zend_float(): Passing null to parameter #1 ($param) of type float is deprecated in %s on line %d
float(0)
NULL

Deprecated: zend_float_slow_zpp(): Passing null to parameter #1 ($param) of type float is deprecated in %s on line %d
float(0)
NULL

Deprecated: zend_float(): Passing null to parameter #1 ($param) of type float is deprecated in %s on line %d
float(0)
NULL

Deprecated: zend_float_slow_zpp(): Passing null to parameter #1 ($param) of type float is deprecated in %s on line %d
float(0)
NULL
Using false:
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
Using true:
float(1)
float(1)
float(1)
float(1)
float(1)
float(1)
float(1)
float(1)
Using 42:
float(42)
float(42)
float(42)
float(42)
float(42)
float(42)
float(42)
float(42)
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
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
Using '15':
float(15)
float(15)
float(15)
float(15)
float(15)
float(15)
float(15)
float(15)
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
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
Using anon class name:
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
Using []:
TypeError: zend_float(): Argument #1 ($param) must be of type float, array given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, array given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, array given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, array given
TypeError: zend_float(): Argument #1 ($param) must be of type float, array given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, array given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, array given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, array given
Using new stdClass():
TypeError: zend_float(): Argument #1 ($param) must be of type float, stdClass given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, stdClass given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, stdClass given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, stdClass given
TypeError: zend_float(): Argument #1 ($param) must be of type float, stdClass given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, stdClass given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, stdClass given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, stdClass given
Using new S():
TypeError: zend_float(): Argument #1 ($param) must be of type float, S given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, S given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, S given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, S given
TypeError: zend_float(): Argument #1 ($param) must be of type float, S given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, S given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, S given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, S given
Using STDOUT:
TypeError: zend_float(): Argument #1 ($param) must be of type float, resource given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, resource given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, resource given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, resource given
TypeError: zend_float(): Argument #1 ($param) must be of type float, resource given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, resource given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, resource given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, resource given
