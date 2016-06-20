--TEST--
typeof() correctly recognizes object vars
--FILE--
<?php

use Php\Test\Typeof\TestClass;

require __DIR__ . '/typeof_TestClass.php';

$data = [
	(object) [],
	new TestClass,
	function () {},
	unserialize('O:1:"A":0:{}'),
];

echo "\n*** typeof test: object ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum));
}

echo "\n*** typeof test extended: object ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum, true));
}

?>
--EXPECTF--

*** typeof test: object ***
string(6) "object"
string(6) "object"
string(6) "object"
string(6) "object"

*** typeof test extended: object ***
string(24) "object of class stdClass"
string(41) "object of class Php\Test\Typeof\TestClass"
string(23) "object of class Closure"
string(38) "object of class __PHP_Incomplete_Class"
