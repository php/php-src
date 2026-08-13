--TEST--
Test Z_PARAM_OBJ_OF_CLASS_OR_LONG() and Z_PARAM_OBJ_OF_CLASS_OR_LONG_OR_NULL() (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_obj_stdclass_or_int($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_obj_stdclass_or_int_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:

Deprecated: zend_obj_stdclass_or_int(): Passing null to parameter #1 ($param) of type stdClass|int|null is deprecated in %s on line %d
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

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)

Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)
Using 'string':
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, string given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, string given
Using '15':
int(15)
int(15)
Using '56.7':

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)

Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)
Using 'stdClass':
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, string given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, string given
Using anon class name:
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, string given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, string given
Using []:
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, array given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, array given
Using new stdClass():
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
Using new S():
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, S given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, S given
Using STDOUT:
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, resource given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, resource given
