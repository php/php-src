--TEST--
Test Z_PARAM_OBJ_OF_CLASS_OR_LONG() and Z_PARAM_OBJ_OF_CLASS_OR_LONG_OR_NULL() (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

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
--EXPECT--
Using null:
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, null given
NULL
Using false:
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, false given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, false given
Using true:
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, true given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, true given
Using 42:
int(42)
int(42)
Using 73.5:
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, float given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, float given
Using 'string':
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, string given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, string given
Using '15':
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, string given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, string given
Using '56.7':
TypeError: zend_obj_stdclass_or_int(): Argument #1 ($param) must be of type stdClass|int, string given
TypeError: zend_obj_stdclass_or_int_or_null(): Argument #1 ($param) must be of type stdClass|int|null, string given
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
