--TEST--
Test class string ZPP specifier
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_class_name($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_class_name_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_class_name_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_class_name_or_null_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	$ref =& $type;
	try {
		var_dump(zend_class_name($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_class_name_or_null($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_class_name_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_class_name_or_null_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, null given
NULL
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name,  given
NULL
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, null given
NULL
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name,  given
NULL
Using false:
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, false given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, false given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name,  given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null,  given
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, false given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, false given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name,  given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null,  given
Using true:
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, true given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, true given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 1 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 1 given
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, true given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, true given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 1 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 1 given
Using 42:
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, int given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, int given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 42 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 42 given
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, int given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, int given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 42 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 42 given
Using 73.5:
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, float given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, float given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 73.5 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 73.5 given
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, float given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, float given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 73.5 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 73.5 given
Using 'string':
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, string given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name, string given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, string given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, string given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, string given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name, string given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, string given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, string given
Using '15':
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, 15 given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name, 15 given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 15 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 15 given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, 15 given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name, 15 given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 15 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 15 given
Using '56.7':
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, 56.7 given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name, 56.7 given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 56.7 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 56.7 given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, 56.7 given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name, 56.7 given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, 56.7 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, 56.7 given
Using 'stdClass':
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
Using anon class name:
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
Using []:
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, array given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, array given

Warning: Array to string conversion in %s on line %d
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, Array given

Warning: Array to string conversion in %s on line %d
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, Array given
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, array given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, array given

Warning: Array to string conversion in %s on line %d
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, Array given

Warning: Array to string conversion in %s on line %d
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, Array given
Using new stdClass():
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, stdClass given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, stdClass given
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, stdClass given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, stdClass given
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Using new S():
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, S given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, S given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, S class given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, S class given
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, S given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, S given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, S class given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, S class given
Using STDOUT:
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, resource given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, Resource id #2 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, Resource id #2 given
TypeError: zend_class_name(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be of type ?string, resource given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, Resource id #2 given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, Resource id #2 given
