--TEST--
Test int ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

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
--EXPECTF--
Using null:

Deprecated: zend_int(): Passing null to parameter #1 ($param) of type int is deprecated in %s on line %d
int(0)
NULL

Deprecated: zend_int_slow_zpp(): Passing null to parameter #1 ($param) of type int is deprecated in %s on line %d
int(0)
NULL

Deprecated: zend_int(): Passing null to parameter #1 ($param) of type int is deprecated in %s on line %d
int(0)
NULL

Deprecated: zend_int_slow_zpp(): Passing null to parameter #1 ($param) of type int is deprecated in %s on line %d
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

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)
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
int(15)
int(15)
int(15)
int(15)
int(15)
int(15)
int(15)
int(15)
Using '56.7':

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)
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
