--TEST--
Test Z_PARAM_OBJ_OR_CLASS_NAME() and Z_PARAM_OBJ_OR_CLASS_NAME_OR_NULL (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_obj_or_class_name($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_obj_or_class_name_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, null given
NULL
Using false:
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, false given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, false given
Using true:
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, true given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, true given
Using 42:
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, int given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, int given
Using 73.5:
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, float given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, float given
Using 'string':
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, string given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, string given
Using '15':
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, string given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, string given
Using '56.7':
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, string given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, string given
Using 'stdClass':
string(8) "stdClass"
string(8) "stdClass"
Using anon class name:
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
Using []:
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, array given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, array given
Using new stdClass():
string(8) "stdClass"
string(8) "stdClass"
Using new S():
string(1) "S"
string(1) "S"
Using STDOUT:
TypeError: zend_obj_or_class_name(): Argument #1 ($param) must be an object or a valid class name, resource given
TypeError: zend_obj_or_class_name_or_null(): Argument #1 ($param) must be an object, a valid class name, or null, resource given
