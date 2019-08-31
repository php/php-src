--TEST--
Test return type and value for expected input log10()
--INI--
precision = 14
--FILE--
<?php
/*
 * proto float log10(float number)
 * Function is implemented in ext/standard/math.c
*/

$file_path = __DIR__;
require($file_path."/allowed_rounding_error.inc");

$arg_0 = 1.0;
$arg_1 = 10.0;
$arg_2 = 100.0;

echo "log10 $arg_0 = ";
$r0 = log10($arg_0);
var_dump($r0);
if (allowed_rounding_error($r0 ,0.0 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "log10 $arg_1 = ";
$r1 = log10($arg_1);
var_dump($r1);
if (allowed_rounding_error($r1 ,1.0 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "log10 $arg_2 = ";
$r2 = log10($arg_2);
var_dump($r2);
if (allowed_rounding_error($r2 ,2.0 )) {
	echo "Pass\n";
}
else {
	echo "Fail\n";
}
?>
--EXPECTF--
log10 1 = float(%f)
Pass
log10 10 = float(%f)
Pass
log10 100 = float(%f)
Pass
