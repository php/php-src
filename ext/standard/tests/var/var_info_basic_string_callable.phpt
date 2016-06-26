--TEST--
var_info() correctly identifies callable strings
--FILE--
<?php

function fn() {}

final class TestClass {
	public static function fn() {}
}

$data = [
	'fn',
	"fn",
	'TestClass::fn',
	"TestClass::fn",
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'callable string' || var_dump($type);
}

?>
--EXPECT--
