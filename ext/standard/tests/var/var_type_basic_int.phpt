--TEST--
var_type() correctly identifies int variables
--FILE--
<?php

$data = [
	PHP_INT_MIN,
	-001,
	-0x1,
	-0b1,
	-1,
	-0,
	0,
	1,
	0b1,
	0x1,
	001,
	PHP_INT_MAX,
];

foreach ($data as $datum) {
	$type = var_type($datum);

	$type === 'int' || var_dump($type);
}

?>
--EXPECT--
