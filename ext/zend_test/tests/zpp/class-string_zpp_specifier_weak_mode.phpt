--TEST--
Test class string ZPP specifier
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	/* Use zend_object_init_with_constructor() function as it used Z_PARAM_CLASS */
	try {
		var_dump(zend_object_init_with_constructor($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Using null:

Deprecated: zend_object_init_with_constructor(): Passing null to parameter #1 ($class) of type string is deprecated in %s on line %d
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name,  given
Using false:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name,  given
Using true:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 1 given
Using 42:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 42 given
Using 73.5:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 73.5 given
Using 'string':
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, string given
Using '15':
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 15 given
Using '56.7':
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 56.7 given
Using 'stdClass':
object(stdClass)#4 (0) {
}
Using anon class name:
object(class@anonymous)#4 (0) {
}
Using []:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, array given
Using new stdClass():
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, stdClass given
Using new S():
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, S class given
Using STDOUT:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, resource given
