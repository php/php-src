--TEST--
bug 7715, floats value with integer or incomplete input
--FILE--
<?php
$data = array(
	'.23',
	'-42',
	'+42',
	'.4',
	'-.4',
	'1000000000000',
	'-1000000000000'
);
foreach ($data as $val) {
	$res = filter_data($val, FILTER_VALIDATE_FLOAT);
	var_dump($res);
}
echo "\n";
?>
--EXPECTF--	
float(0.23)
float(-42)
float(42)
float(0.4)
float(-0.4)
float(1.0E+12)
float(-1.0E+12)
