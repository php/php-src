--TEST--
Test return type and value for expected input atan()
--INI--
precision = 14
--FILE--
<?php
/*
 * proto float atan(float number)
 * Function is implemented in ext/standard/math.c
*/

$file_path = dirname(__FILE__);
require($file_path."/allowed_rounding_error.inc");

echo "atan 1.7320508075689 = ";
$atan1 = 360 * atan(1.7320508075689) / (2.0 * M_PI);
var_dump($atan1);
if (allowed_rounding_error($atan1 ,60 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "atan 0.57735026918963 = ";
$atan2 = 360 * atan(0.57735026918963) / (2.0 * M_PI);
var_dump($atan2);
if (allowed_rounding_error($atan2 ,30 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

?>
--EXPECTF--
atan 1.7320508075689 = float(%f)
Pass
atan 0.57735026918963 = float(%f)
Pass
