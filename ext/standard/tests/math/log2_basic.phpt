--TEST--
Test return type and value for expected input log2()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float log2(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$file_path = dirname(__FILE__);
require($file_path."/allowed_rounding_error.inc");

$arg_0 = 1.0;
$arg_1 = 10.0;
$arg_2 = 100.0;

echo "log2 $arg_0 = ";
$r0 = log2($arg_0);
var_dump($r0);
if (allowed_rounding_error($r0 ,0.0 )) {
	echo "Pass\n";
} else {
	echo "Fail\n";
}

echo "log2 $arg_1 = ";
$r1 = log2($arg_1);
var_dump($r1);
if (allowed_rounding_error($r1, 3.3219280948874)) {
	echo "Pass\n";
} else {
	echo "Fail\n";
}

echo "log2 $arg_2 = ";
$r2 = log2($arg_2);
var_dump($r2);
if (allowed_rounding_error($r2, 6.6438561897747)) {
	echo "Pass\n";
} else {
	echo "Fail\n";
}
?>
--EXPECTF--
log2 1 = float(%f)
Pass
log2 10 = float(%f)
Pass
log2 100 = float(%f)
Pass
