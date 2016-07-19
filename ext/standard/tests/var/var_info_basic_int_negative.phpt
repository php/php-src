--TEST--
var_info() correctly identifies negative ints
--FILE--
<?php

$data = [
	PHP_INT_MIN,
	-001,
	-0x1,
	-0b1,
	-1,
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'negative int' || var_dump($type);
}

?>
--EXPECT--
