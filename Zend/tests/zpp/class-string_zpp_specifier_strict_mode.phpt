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
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, null given
NULL
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, null given
NULL
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, null given
NULL
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, null given
NULL
Using false:
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, false given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, false given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, false given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, false given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, false given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, false given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, false given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, false given
Using true:
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, true given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, true given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, true given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, true given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, true given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, true given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, true given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, true given
Using 42:
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, int given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, int given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, int given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, int given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, int given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, int given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, int given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, int given
Using 73.5:
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, float given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, float given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, float given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, float given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, float given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, float given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, float given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, float given
Using 'string':
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, "string" given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, "string" given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, "string" given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, "string" given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, "string" given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, "string" given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, "string" given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, "string" given
Using '15':
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, "15" given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, "15" given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, "15" given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, "15" given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, "15" given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, "15" given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, "15" given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, "15" given
Using '56.7':
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, "56.7" given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, "56.7" given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, "56.7" given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, "56.7" given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, "56.7" given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, "56.7" given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, "56.7" given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, "56.7" given
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
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, array given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, array given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, array given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, array given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, array given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, array given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, array given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, array given
Using new stdClass():
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, stdClass given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, stdClass given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, stdClass given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, stdClass given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, stdClass given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, stdClass given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, stdClass given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, stdClass given
Using new S():
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, S given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, S given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, S given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, S given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, S given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, S given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, S given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, S given
Using STDOUT:
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, resource given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, resource given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, resource given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, resource given
TypeError: zend_class_name(): Argument #1 ($param) must be a valid class name, resource given
TypeError: zend_class_name_or_null(): Argument #1 ($param) must be a valid class name or null, resource given
TypeError: zend_class_name_slow_zpp(): Argument #1 ($param) must be a valid class name, resource given
TypeError: zend_class_name_or_null_slow_zpp(): Argument #1 ($param) must be a valid class name or null, resource given
