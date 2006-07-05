--TEST--
filter_data() and FLOAT
--FILE--
<?php

$floats = array(
'1.234   ',
'   1.234',
'1.234'	,
'1.2e3',
'7E3',
'7E3     ',
'  7E3     ',
'  7E-3     '
);

foreach ($floats as $float) {
	$out = filter_data($float, FILTER_VALIDATE_FLOAT);
	var_dump($out);
}

$floats = array(
'1.234   '	=> ',',
'1,234'		=> ',',
'   1.234'	=> '.',
'1.234'		=> '..',
'1.2e3'		=> ','
);

echo "\ncustom decimal:\n";
foreach ($floats as $float => $dec) {
	$out = filter_data($float, FILTER_VALIDATE_FLOAT, array('decimal' => $dec));
	var_dump($out);
}

?>
--EXPECTF--
float(1.234)
float(1.234)
float(1.234)
float(1200)
float(7000)
float(7000)
float(7000)
float(0.007)

custom decimal:
bool(false)
float(1.234)
float(1.234)

Warning: filter_data(): decimal separator must be one char in %s on line %d
bool(false)
bool(false)
