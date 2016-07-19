--TEST--
var_info() correctly identifies objects
--FILE--
<?php

final class TestClass {}

$data = [
	[(object) [], 'stdClass'],
	[function () {}, 'Closure'],
	[new TestClass, 'TestClass'],
	[unserialize('O:1:"A":0:{}'), '__PHP_Incomplete_Class'],
];

foreach ($data as list($datum, $class)) {
	$type = var_info($datum);

	$type === "object of class {$class}" || var_dump($type);
}

?>
--EXPECT--
