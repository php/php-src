--TEST--
Test Z_PARAM_OBJ() and Z_PARAM_OBJ_OR_NULL (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_obj($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_obj_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	$ref =& $type;
	try {
		var_dump(zend_obj($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_obj_or_null($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECT--
Using null:
TypeError: zend_obj(): Argument #1 ($param) must be of type object, null given
NULL
TypeError: zend_obj(): Argument #1 ($param) must be of type object, null given
NULL
Using false:
TypeError: zend_obj(): Argument #1 ($param) must be of type object, false given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, false given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, false given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, false given
Using true:
TypeError: zend_obj(): Argument #1 ($param) must be of type object, true given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, true given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, true given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, true given
Using 42:
TypeError: zend_obj(): Argument #1 ($param) must be of type object, int given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, int given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, int given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, int given
Using 73.5:
TypeError: zend_obj(): Argument #1 ($param) must be of type object, float given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, float given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, float given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, float given
Using 'string':
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
Using '15':
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
Using '56.7':
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
Using 'stdClass':
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
Using anon class name:
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, string given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, string given
Using []:
TypeError: zend_obj(): Argument #1 ($param) must be of type object, array given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, array given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, array given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, array given
Using new stdClass():
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
Using new S():
object(S)#3 (0) {
}
object(S)#3 (0) {
}
object(S)#3 (0) {
}
object(S)#3 (0) {
}
Using STDOUT:
TypeError: zend_obj(): Argument #1 ($param) must be of type object, resource given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, resource given
TypeError: zend_obj(): Argument #1 ($param) must be of type object, resource given
TypeError: zend_obj_or_null(): Argument #1 ($param) must be of type ?object, resource given
