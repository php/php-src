--TEST--
Test float ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

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
--EXPECT--
Using null:
TypeError: zend_float(): Argument #1 ($param) must be of type float, null given
NULL
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, null given
NULL
TypeError: zend_float(): Argument #1 ($param) must be of type float, null given
NULL
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, null given
NULL
Using false:
TypeError: zend_float(): Argument #1 ($param) must be of type float, false given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, false given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, false given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, false given
TypeError: zend_float(): Argument #1 ($param) must be of type float, false given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, false given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, false given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, false given
Using true:
TypeError: zend_float(): Argument #1 ($param) must be of type float, true given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, true given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, true given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, true given
TypeError: zend_float(): Argument #1 ($param) must be of type float, true given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, true given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, true given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, true given
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
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
Using '56.7':
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null(): Argument #1 ($param) must be of type ?float, string given
TypeError: zend_float_slow_zpp(): Argument #1 ($param) must be of type float, string given
TypeError: zend_float_or_null_slow_zpp(): Argument #1 ($param) must be of type ?float, string given
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
