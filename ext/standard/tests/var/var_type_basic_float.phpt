--TEST--
var_type() correctly identifies float variables
--FILE--
<?php

$data = [
	-NAN,
	-INF,
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
	-0.0,
	0.0,
	.5,
	1.0,
	1e5,
	1E5,
	.5e6,
	.5e+6,
	.512E6,
	.512E-6,
	.512E+6,
	+.512E-6,
	INF,
	NAN,
];

foreach ($data as $datum) {
	$type = var_type($datum);

	$type === 'float' || var_dump($type);
}

?>
--EXPECT--
