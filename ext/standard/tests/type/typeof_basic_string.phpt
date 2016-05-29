--TEST--
typeof() correctly recognizes string vars
--FILE--
<?php

require __DIR__ . '/typeof_TestClass.php';

$data = [
	'',
	'string',
	'1',
	'1.0',
	'10abc',
	'abc10',
	'strlen',
	'Php\Test\Typeof\TestClass::staticMethod',
	'Php\Test\Typeof\TestClass::instanceMethod',
];

echo "\n*** typeof test: string ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum));
}

echo "\n*** typeof test extended: string ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum, true));
}

?>
--EXPECTF--

*** typeof test: string ***
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"

*** typeof test extended: string ***
string(6) "string"
string(6) "string"
string(14) "numeric string"
string(14) "numeric string"
string(6) "string"
string(6) "string"
string(15) "callable string"
string(15) "callable string"

Deprecated: Non-static method Php\Test\Typeof\TestClass::instanceMethod() should not be called statically in C:\php-sdk\phpdev\vc14\x64\php-src\ext\standard\tests\type\typeof_basic_string.php on line 24
string(15) "callable string"
