--TEST--
var_info() correctly identifies zero floats
--FILE--
<?php

$data = [
	-0.0,
	0.0,
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'zero float' || var_dump($type);
}

?>
--EXPECT--
