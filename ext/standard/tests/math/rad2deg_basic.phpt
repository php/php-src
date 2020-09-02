--TEST--
Test return type and value for expected input rad2deg()
--INI--
precision = 14
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/

$file_path = __DIR__;
require($file_path."/allowed_rounding_error.inc");

$arg_0 = 0.0;
$arg_1 = 1.570796327;
$arg_2 = 3.141592654;
$arg_3 = 6.283185307;

echo "rad2deg $arg_0= ";
$r0 = rad2deg($arg_0);
var_dump($r0);
if (allowed_rounding_error($r0 ,0 )) {
    echo "Pass\n";
}
else {
    echo "Fail\n";
}
echo "rad2deg $arg_1 = ";
$r1 = rad2deg($arg_1);
var_dump($r1);
if (allowed_rounding_error($r1 ,90.000000011752)) {
    echo "Pass\n";
}
else {
    echo "Fail\n";
}
echo "rad2deg $arg_2  = ";
$r2 = rad2deg($arg_2);
var_dump($r2);
if (allowed_rounding_error($r2 ,180.0000000235 )) {
    echo "Pass\n";
}
else {
    echo "Fail\n";
}
echo "rad2deg $arg_3 = ";
$r3 = rad2deg($arg_3);
var_dump($r3);
if (allowed_rounding_error($r3 ,359.99999998971 )) {
    echo "Pass\n";
}
else {
    echo "Fail\n";
}
?>
--EXPECTF--
rad2deg 0= float(%f)
Pass
rad2deg 1.570796327 = float(%f)
Pass
rad2deg 3.141592654  = float(%f)
Pass
rad2deg 6.283185307 = float(%f)
Pass
