--TEST--
var_info() correctly identifies callable arrays
--FILE--
<?php

final class TestClass {
	public static function f() {}
}

$data = [
	array('TestClass', 'f'),
	['TestClass', 'f'],
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'callable array' || var_dump($type);
}

?>
--EXPECT--
