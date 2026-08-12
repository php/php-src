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
	/* Use zend_object_init_with_constructor() function as it used Z_PARAM_CLASS */
	try {
		var_dump(zend_object_init_with_constructor($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECT--
Using null:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, null given
Using false:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, false given
Using true:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, true given
Using 42:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, int given
Using 73.5:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, float given
Using 'string':
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, string given
Using []:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, array given
Using new stdClass():
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, stdClass given
Using new S():
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, S given
Using STDOUT:
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, resource given
