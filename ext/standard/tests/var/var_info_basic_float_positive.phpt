--TEST--
var_info() correctly identifies positive floats
--FILE--
<?php

$data = [
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
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'positive float' || var_dump($type);
}

?>
--EXPECT--
