--TEST--
Test resource ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";
	try {
		var_dump(zend_resource($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_resource_or_null($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_resource_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_resource_or_null_slow_zpp($type));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	$ref =& $type;
	try {
		var_dump(zend_resource($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_resource_or_null($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_resource_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
	try {
		var_dump(zend_resource_or_null_slow_zpp($ref));
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

?>
--EXPECT--
Using null:
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, null given
NULL
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, null given
NULL
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, null given
NULL
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, null given
NULL
Using false:
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, false given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, false given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, false given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, false given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, false given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, false given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, false given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, false given
Using true:
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, true given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, true given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, true given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, true given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, true given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, true given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, true given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, true given
Using 42:
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, int given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, int given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, int given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, int given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, int given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, int given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, int given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, int given
Using 73.5:
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, float given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, float given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, float given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, float given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, float given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, float given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, float given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, float given
Using 'string':
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
Using '15':
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
Using '56.7':
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
Using 'stdClass':
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
Using anon class name:
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, string given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, string given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, string given
Using []:
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, array given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, array given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, array given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, array given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, array given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, array given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, array given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, array given
Using new stdClass():
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, stdClass given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, stdClass given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, stdClass given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, stdClass given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, stdClass given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, stdClass given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, stdClass given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, stdClass given
Using new S():
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, S given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, S given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, S given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, S given
TypeError: zend_resource(): Argument #1 ($param) must be of type resource, S given
TypeError: zend_resource_or_null(): Argument #1 ($param) must be of type resource or null, S given
TypeError: zend_resource_slow_zpp(): Argument #1 ($param) must be of type resource, S given
TypeError: zend_resource_or_null_slow_zpp(): Argument #1 ($param) must be of type resource or null, S given
Using STDOUT:
resource(2) of type (stream)
resource(2) of type (stream)
resource(2) of type (stream)
resource(2) of type (stream)
resource(2) of type (stream)
resource(2) of type (stream)
resource(2) of type (stream)
resource(2) of type (stream)
