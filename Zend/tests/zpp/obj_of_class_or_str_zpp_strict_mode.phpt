--TEST--
Test Z_PARAM_OBJ_OF_CLASS_OR_STR() and Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_obj_stdclass_or_string($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_obj_stdclass_or_string_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:
TypeError: zend_obj_stdclass_or_string(): Argument #1 ($param) must be of type stdClass|string, null given
NULL
Using false:
TypeError: zend_obj_stdclass_or_string(): Argument #1 ($param) must be of type stdClass|string, false given
TypeError: zend_obj_stdclass_or_string_or_null(): Argument #1 ($param) must be of type stdClass|string|null, false given
Using true:
TypeError: zend_obj_stdclass_or_string(): Argument #1 ($param) must be of type stdClass|string, true given
TypeError: zend_obj_stdclass_or_string_or_null(): Argument #1 ($param) must be of type stdClass|string|null, true given
Using 42:
TypeError: zend_obj_stdclass_or_string(): Argument #1 ($param) must be of type stdClass|string, int given
TypeError: zend_obj_stdclass_or_string_or_null(): Argument #1 ($param) must be of type stdClass|string|null, int given
Using 73.5:
TypeError: zend_obj_stdclass_or_string(): Argument #1 ($param) must be of type stdClass|string, float given
TypeError: zend_obj_stdclass_or_string_or_null(): Argument #1 ($param) must be of type stdClass|string|null, float given
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
TypeError: zend_obj_stdclass_or_string(): Argument #1 ($param) must be of type stdClass|string, array given
TypeError: zend_obj_stdclass_or_string_or_null(): Argument #1 ($param) must be of type stdClass|string|null, array given
Using new stdClass():
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
Using new S():
TypeError: zend_obj_stdclass_or_string(): Argument #1 ($param) must be of type stdClass|string, S given
TypeError: zend_obj_stdclass_or_string_or_null(): Argument #1 ($param) must be of type stdClass|string|null, S given
Using STDOUT:
TypeError: zend_obj_stdclass_or_string(): Argument #1 ($param) must be of type stdClass|string, resource given
TypeError: zend_obj_stdclass_or_string_or_null(): Argument #1 ($param) must be of type stdClass|string|null, resource given
