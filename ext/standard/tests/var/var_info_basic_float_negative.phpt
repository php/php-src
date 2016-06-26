--TEST--
var_info() correctly identifies negative floats
--FILE--
<?php

$data = [
	-.512E+6,
	-.512E6,
	-.5e+6,
	-.5e-6,
	-.5e6,
	-1E5,
	-1e5,
	-1.5,
	-1.0,
	-.5,
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'negative float' || var_dump($type);
}

?>
--EXPECT--
