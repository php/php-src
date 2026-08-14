--TEST--
Test Z_PARAM_NUMBER_OR_STR() and Z_PARAM_NUMBER_OR_STR_OR_NULL (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

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
TypeError: zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, null given
NULL
Using false:
TypeError: zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, false given
TypeError: zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, false given
Using true:
TypeError: zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, true given
TypeError: zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, true given
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
TypeError: zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, S given
TypeError: zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, S given
Using STDOUT:
TypeError: zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, resource given
TypeError: zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, resource given
