--TEST--
Test Z_PARAM_OBJ_OF_CLASS_OR_STR() and Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_string_or_stdclass($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_string_or_stdclass_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:

Deprecated: zend_string_or_stdclass(): Passing null to parameter #1 ($param) of type string is deprecated in %s on line %d
string(0) ""
NULL
Using false:
string(0) ""
string(0) ""
Using true:
string(1) "1"
string(1) "1"
Using 42:
string(2) "42"
string(2) "42"
Using 73.5:
string(4) "73.5"
string(4) "73.5"
Using 'string':
string(6) "string"
string(6) "string"
Using []:
TypeError: zend_string_or_stdclass(): Argument #1 ($param) must be of type stdClass|string, array given
TypeError: zend_string_or_stdclass_or_null(): Argument #1 ($param) must be of type stdClass|string|null, array given
Using new stdClass():
object(stdClass)#1 (0) {
}
object(stdClass)#1 (0) {
}
Using new S():
string(7) "S class"
string(7) "S class"
Using STDOUT:
TypeError: zend_string_or_stdclass(): Argument #1 ($param) must be of type stdClass|string, resource given
TypeError: zend_string_or_stdclass_or_null(): Argument #1 ($param) must be of type stdClass|string|null, resource given
