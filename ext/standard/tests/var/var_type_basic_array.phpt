--TEST--
var_type() correctly identifies array variables
--FILE--
<?php

$data = [
	array(),
	[],
	array(null),
	[null],
	array(4, 2),
	[4, 2],
	array('p', 'h' => 'p'),
	['p', 'h' => 'p'],
];

foreach ($data as $datum) {
	$type = var_type($datum);

	$type === 'array' || var_dump($type);
}

?>
--EXPECT--
