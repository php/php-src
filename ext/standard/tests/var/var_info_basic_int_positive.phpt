--TEST--
var_info() correctly identifies positive ints
--FILE--
<?php

$data = [
	1,
	0b1,
	0x1,
	001,
	PHP_INT_MAX,
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'positive int' || var_dump($type);
}

?>
--EXPECT--
