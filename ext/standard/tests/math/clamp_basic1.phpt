--TEST--
Test clamp() function : basic functionality 01
--FILE--
<?php
echo "*** Testing clamp() : basic functionality 01 ***\n";

var_dump(clamp(0, 1, 2));
var_dump(clamp(2, 1, 3));
var_dump(clamp(3, 1, 4));
var_dump(clamp(5, 1, 4));
var_dump(clamp(5, 1, 5));
var_dump(clamp(0.0, 1.0, 2.0));
var_dump(clamp(1.5, 1.0, 2.0));
var_dump(clamp(2.0, 1.0, 2.5));
var_dump(clamp(3.0, 1.0, 2.5));
var_dump(clamp(0, 3.0, 3.5));

try {
    var_dump(clamp(5, 5, 1));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(clamp(1, 3.5, 3.0));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(clamp(0, null, 3.5));

try {
    var_dump(clamp(5, 1, null));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(clamp(-2, -5, 1));
var_dump(clamp(-2.3, -3.0, 3.5));
?>
--EXPECTF--
*** Testing clamp() : basic functionality 01 ***
int(1)
int(2)
int(3)
int(4)
int(5)
float(1)
float(1.5)
float(2)
float(2.5)
float(3)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)

Deprecated: clamp(): Passing null to parameter #2 ($min) of type int|float is deprecated in %s on line %d
int(0)

Deprecated: clamp(): Passing null to parameter #3 ($max) of type int|float is deprecated in %s on line %d
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
int(-2)
float(-2.3)
