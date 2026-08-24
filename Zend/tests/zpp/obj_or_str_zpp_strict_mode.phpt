--TEST--
Test Z_PARAM_OBJ_OR_STR() and Z_PARAM_OBJ_OR_STR_OR_NULL (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_string_or_object($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_string_or_object_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:
TypeError: zend_string_or_object(): Argument #1 ($param) must be of type object|string, null given
NULL
Using false:
TypeError: zend_string_or_object(): Argument #1 ($param) must be of type object|string, false given
TypeError: zend_string_or_object_or_null(): Argument #1 ($param) must be of type object|string|null, false given
Using true:
TypeError: zend_string_or_object(): Argument #1 ($param) must be of type object|string, true given
TypeError: zend_string_or_object_or_null(): Argument #1 ($param) must be of type object|string|null, true given
Using 42:
TypeError: zend_string_or_object(): Argument #1 ($param) must be of type object|string, int given
TypeError: zend_string_or_object_or_null(): Argument #1 ($param) must be of type object|string|null, int given
Using 73.5:
TypeError: zend_string_or_object(): Argument #1 ($param) must be of type object|string, float given
TypeError: zend_string_or_object_or_null(): Argument #1 ($param) must be of type object|string|null, float given
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
TypeError: zend_string_or_object(): Argument #1 ($param) must be of type object|string, array given
TypeError: zend_string_or_object_or_null(): Argument #1 ($param) must be of type object|string|null, array given
Using new stdClass():
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
Using new S():
object(S)#3 (0) {
}
object(S)#3 (0) {
}
Using STDOUT:
TypeError: zend_string_or_object(): Argument #1 ($param) must be of type object|string, resource given
TypeError: zend_string_or_object_or_null(): Argument #1 ($param) must be of type object|string|null, resource given
