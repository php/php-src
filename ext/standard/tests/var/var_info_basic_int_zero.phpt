--TEST--
var_info() correctly identifies zero ints
--FILE--
<?php

$data = [
	-0,
	0,
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'zero int' || var_dump($type);
}

?>
--EXPECT--
