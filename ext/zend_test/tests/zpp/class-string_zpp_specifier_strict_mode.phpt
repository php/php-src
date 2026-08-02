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
--EXPECT--
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, null given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, false given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, true given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, int given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, float given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be a valid class name, string given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, array given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, stdClass given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, S given
TypeError: zend_object_init_with_constructor(): Argument #1 ($class) must be of type string, resource given
