--TEST--
Test Z_PARAM_NUMBER_OR_STR() and Z_PARAM_NUMBER_OR_STR_OR_NULL (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_number_or_string($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_number_or_string_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:

Deprecated: zend_number_or_string(): Passing null to parameter #1 ($param) of type string|int|float is deprecated in %s on line %d
int(0)
NULL
Using false:
int(0)
int(0)
Using true:
int(1)
int(1)
Using 42:
int(42)
int(42)
Using 73.5:
float(73.5)
float(73.5)
Using 'string':
string(6) "string"
string(6) "string"
Using '15':
string(2) "15"
string(2) "15"
Using '56.7':
string(4) "56.7"
string(4) "56.7"
Using 'stdClass':
string(8) "stdClass"
string(8) "stdClass"
Using anon class name:
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
Using []:
TypeError: zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, array given
TypeError: zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, array given
Using new stdClass():
TypeError: zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, stdClass given
TypeError: zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, stdClass given
Using new S():
string(7) "S class"
string(7) "S class"
Using STDOUT:
TypeError: zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, resource given
TypeError: zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, resource given
