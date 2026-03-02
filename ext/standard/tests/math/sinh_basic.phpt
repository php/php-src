--TEST--
Test return type and value for expected input sinh()
--INI--
precision = 14
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/

$file_path = __DIR__;
require($file_path."/allowed_rounding_error.inc");

echo "sinh .5  = ";
var_dump(sinh(0.5));
if (allowed_rounding_error(sinh(0.5),0.52109530549375)){
    echo "Pass\n";
}
else {
    echo "Fail\n";
}

echo "sinh -0.5  = ";
var_dump(sinh(-0.5));
if (allowed_rounding_error(sinh(-0.5), -0.52109530549375)){
    echo "Pass\n";
}
else {
    echo "Fail\n";
}

echo "sinh 3  = ";
var_dump(sinh(3.0));
if (allowed_rounding_error(sinh(3.0), 10.01787492741)){
    echo "Pass\n";
}
else {
    echo "Fail\n";
}

echo "sinh -3  = ";
var_dump(sinh(-3.0));
if (allowed_rounding_error(sinh(-3.0), -10.01787492741)){
    echo "Pass\n";
}
else {
    echo "Fail\n";
}

?>
--EXPECTF--
sinh .5  = float(%f)
Pass
sinh -0.5  = float(%f)
Pass
sinh 3  = float(%f)
Pass
sinh -3  = float(%f)
Pass
