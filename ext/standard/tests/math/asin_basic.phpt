--TEST--
Test return type and value for expected input asin()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float asin(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$file_path = dirname(__FILE__);
require($file_path."/allowed_rounding_error.inc");


//output is in degrees

echo "asin .5  = ";
$asinv1 = 360.0  * asin(0.5) / (2.0 * M_PI );
var_dump($asinv1);
if (allowed_rounding_error($asinv1 ,30 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "asin 0.86602540378444 = ";
$asinv2 = 360.0  * asin(0.86602540378444) / (2.0 * M_PI );
var_dump($asinv2);
if (allowed_rounding_error($asinv2 ,60 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "asin 1.0 = ";
$asinv3 = 360.0  * asin(1.0) / (2.0 * M_PI );
var_dump($asinv3);
if (allowed_rounding_error($asinv3 ,90 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}


echo "asin 0.0 = ";
$asinv4 = 360.0  * asin(0.0) / (2.0 * M_PI );
var_dump($asinv4);
if (allowed_rounding_error($asinv4 ,0 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

?>
--EXPECTF--
asin .5  = float(%f)
Pass
asin 0.86602540378444 = float(%f)
Pass
asin 1.0 = float(%f)
Pass
asin 0.0 = float(%f)
Pass
