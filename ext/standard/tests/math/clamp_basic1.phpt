--TEST--
Test clamp() function : basic functionality
--FILE--
<?php
echo "*** Testing clamp() : basic functionality - using integers ***\n";

var_dump(clamp(0, 1, 2));
var_dump(clamp(2, 1, 3));
var_dump(clamp(3, 1, 4));
var_dump(clamp(5, 1, 4));
var_dump(clamp(5, 1, 5));

try {
    var_dump(clamp(5, 5, 1));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
*** Testing clamp() : basic functionality - using integers ***
int(1)
int(2)
int(3)
int(4)
int(5)
clamp(): Argument #2 ($min) cannot be greater than Argument #3 ($max)
