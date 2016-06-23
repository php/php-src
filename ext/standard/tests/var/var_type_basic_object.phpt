--TEST--
var_type() correctly identifies object variables
--FILE--
<?php

final class TestClass {}

$data = [
	(object) [],
	function () {},
	new TestClass,
	unserialize('O:1:"A":0:{}'),
];

foreach ($data as $datum) {
	$type = var_type($datum);

	$type === 'object' || var_dump($type);
}

?>
--EXPECT--
