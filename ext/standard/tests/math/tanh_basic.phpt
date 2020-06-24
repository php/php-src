--TEST--
Test return type and value for expected input tanh()
--INI--
precision = 14
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/
$file_path = __DIR__;
require($file_path."/allowed_rounding_error.inc");


echo "tanh .5  = ";
var_dump(tanh(0.5));
if (allowed_rounding_error(tanh(0.5), 0.46211715726001)) {
    echo "Pass\n";
}
else {
    echo "Fail\n";
}

echo "tanh -0.5  = ";
var_dump(tanh(-0.5));
if (allowed_rounding_error(tanh(-0.5), -0.46211715726001)) {
    echo "Pass\n";
}
else {
    echo "Fail\n";
}

echo "tanh 3  = ";
var_dump(tanh(3.0));
if (allowed_rounding_error(tanh(3.0),0.99505475368673 )) {
    echo "Pass\n";
}
else {
    echo "Fail\n";
}

echo "tanh -3  = ";
var_dump(tanh(-3.0));
if (allowed_rounding_error(tanh(-3.0),-0.99505475368673 )) {
    echo "Pass\n";
}
else {
    echo "Fail\n";
}

?>
--EXPECTF--
tanh .5  = float(%f)
Pass
tanh -0.5  = float(%f)
Pass
tanh 3  = float(%f)
Pass
tanh -3  = float(%f)
Pass
