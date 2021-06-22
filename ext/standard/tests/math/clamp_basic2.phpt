--TEST--
Test clamp() function : basic functionality
--FILE--
<?php
echo "*** Testing clamp() : basic functionality - using floats ***\n";

var_dump(clamp(0.0, 1.0, 2.0));
var_dump(clamp(1.5, 1.0, 2.0));
var_dump(clamp(2.0, 1.0, 2.5));
var_dump(clamp(3.0, 1.0, 2.5));
var_dump(clamp(0, 3.0, 3.5));

try {
    var_dump(clamp(1, 3.5, 3.0));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
*** Testing clamp() : basic functionality - using floats ***
float(1)
float(1.5)
float(2)
float(2.5)
float(3)
clamp(): Argument #2 ($min) cannot be greater than Argument #3 ($max)
