--TEST--
Test bool ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

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
--EXPECTF--
Using null:

Deprecated: zend_bool(): Passing null to parameter #1 ($param) of type bool is deprecated in %s on line %d
bool(false)
NULL

Deprecated: zend_bool_slow_zpp(): Passing null to parameter #1 ($param) of type bool is deprecated in %s on line %d
bool(false)
NULL

Deprecated: zend_bool(): Passing null to parameter #1 ($param) of type bool is deprecated in %s on line %d
bool(false)
NULL

Deprecated: zend_bool_slow_zpp(): Passing null to parameter #1 ($param) of type bool is deprecated in %s on line %d
bool(false)
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
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Using 73.5:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Using 'string':
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Using '15':
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Using '56.7':
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Using 'stdClass':
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Using anon class name:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
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
