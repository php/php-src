--TEST--
Test class string ZPP specifier
--EXTENSIONS--
zend_test
--FILE--
<?php

class S {
	public function __toString(): string {
		return 'S class';
	}
}

$types = [
	null,
	false,
	true,
	42,
	73.5,
	'string',
	[],
	new stdClass(),
	new S(),
	STDOUT,
];

foreach ($types as $type) {
	/* Use zend_object_init_with_constructor() function as it used Z_PARAM_CLASS */
	try {
		var_dump(zend_object_init_with_constructor($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECTF--
Deprecated: zend_object_init_with_constructor(): Passing null to parameter #1 ($class) of type string is deprecated in %s on line %d
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name,  given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name,  given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 1 given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 42 given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 73.5 given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, string given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, array given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, stdClass given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, S class given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, resource given
