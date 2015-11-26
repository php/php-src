--TEST--
Test return type and value for expected input sin()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float sin(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$file_path = dirname(__FILE__);
require($file_path."/allowed_rounding_error.inc");


// Use known values to test

$sixty = M_PI / 3.0;
$thirty = M_PI / 6.0;
$ninety = M_PI /2.0;
$oneeighty = M_PI;
$twoseventy = M_PI * 1.5;
$threesixty = M_PI * 2.0;


echo "sin 30 = ";
var_dump(sin($thirty));
if (allowed_rounding_error(sin($thirty),0.5)) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "sin 60 = ";
var_dump(sin($sixty));
if (allowed_rounding_error(sin($sixty),0.86602540378444)) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "sin 90 = ";
var_dump(sin($ninety));
if (allowed_rounding_error(sin($ninety),1.0)) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "sin 180 = ";
var_dump(sin($oneeighty));
if (allowed_rounding_error(sin($oneeighty),0.0)) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "sin 270 = ";
var_dump(sin($twoseventy));
if (allowed_rounding_error(sin($twoseventy),-1.0)) {
	echo "Pass\n";
}
else {	
	echo "Fail\n";
}

echo "sin 360 = ";
var_dump(sin($threesixty));
if (allowed_rounding_error(sin($threesixty),0.0)) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}
?>
--EXPECTF--
sin 30 = float(%f)
Pass
sin 60 = float(%f)
Pass
sin 90 = float(%f)
Pass
sin 180 = float(%f)
Pass
sin 270 = float(%f)
Pass
sin 360 = float(%f)
Pass
