--TEST--
Test class string ZPP specifier
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

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
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name,  given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name,  given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 1 given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 42 given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, 73.5 given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, string given

Warning: Array to string conversion in %s on line %d
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, Array given
Error: Object of class stdClass could not be converted to string
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, S class given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, Resource id #2 given
