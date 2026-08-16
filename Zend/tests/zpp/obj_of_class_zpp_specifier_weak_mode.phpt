--TEST--
Test Z_PARAM_OBJ_OF_CLASS() and Z_PARAM_OBJ_OF_CLASS_OR_NULL() (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_object_sdtClass($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_object_sdtClass_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_object_sdtClass_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_object_sdtClass_or_null_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	$ref =& $type;
	try {
		var_dump(zend_object_sdtClass($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_object_sdtClass_or_null($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_object_sdtClass_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_object_sdtClass_or_null_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECT--
Using null:
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, null given
NULL
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, null given
NULL
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, null given
NULL
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, null given
NULL
Using false:
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, false given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, false given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, false given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, false given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, false given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, false given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, false given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, false given
Using true:
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, true given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, true given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, true given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, true given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, true given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, true given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, true given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, true given
Using 42:
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, int given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, int given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, int given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, int given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, int given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, int given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, int given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, int given
Using 73.5:
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, float given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, float given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, float given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, float given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, float given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, float given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, float given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, float given
Using 'string':
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
Using '15':
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
Using '56.7':
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
Using 'stdClass':
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
Using anon class name:
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, string given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, string given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, string given
Using []:
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, array given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, array given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, array given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, array given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, array given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, array given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, array given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, array given
Using new stdClass():
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
object(stdClass)#2 (0) {
}
Using new S():
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, S given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, S given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, S given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, S given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, S given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, S given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, S given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, S given
Using STDOUT:
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, resource given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, resource given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, resource given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, resource given
TypeError: zend_object_sdtClass(): Argument #1 ($param) must be of type stdClass, resource given
TypeError: zend_object_sdtClass_or_null(): Argument #1 ($param) must be of type ?stdClass, resource given
TypeError: zend_object_sdtClass_slow_zpp(): Argument #1 ($param) must be of type stdClass, resource given
TypeError: zend_object_sdtClass_or_null_slow_zpp(): Argument #1 ($param) must be of type ?stdClass, resource given
