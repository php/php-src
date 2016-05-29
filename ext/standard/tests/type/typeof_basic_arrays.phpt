--TEST--
typeof() correctly recognizes array vars
--FILE--
<?php

use Php\Test\Typeof\TestClass;

require __DIR__ . '/typeof_TestClass.php';

$data = [
	[],
	[null],
	[4, 2],
	['d' => 'i', 'c' => 't'],
	[TestClass::class, 'staticMethod'],
	[TestClass::class, 'instanceMethod'],
	[new TestClass, 'staticMethod'],
	[new TestClass, 'instanceMethod'],
];

echo "\n*** typeof test: array ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum));
}

echo "\n*** typeof test extended: array ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum, true));
}

?>
--EXPECTF--

*** typeof test: array ***
string(5) "array"
string(5) "array"
string(5) "array"
string(5) "array"
string(5) "array"
string(5) "array"
string(5) "array"
string(5) "array"

*** typeof test extended: array ***
string(5) "array"
string(5) "array"
string(5) "array"
string(5) "array"
string(14) "callable array"

Deprecated: Non-static method Php\Test\Typeof\TestClass::instanceMethod() should not be called statically in %s on line %d
string(14) "callable array"
string(14) "callable array"
string(14) "callable array"
