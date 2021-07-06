--TEST--
Test clamp() function : basic functionality 02
--FILE--
<?php
echo "*** Testing clamp() : basic functionality 02 ***\n";

var_dump(clamp(0, 0.0, 2));
var_dump(clamp(0.0, 0, 2));
try {
    var_dump(clamp(0.0, NAN, 2));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(clamp(1, 0, NAN));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(clamp(0x10, 0x01, 2));
var_dump(clamp(10, "-2.345e1", 2.345e1));
var_dump(clamp(24.0, 027, 25));
var_dump(clamp(2.345e1, "-2.345e1", 2.345e1));
var_dump(clamp(0.0, null, 2));
var_dump(clamp(0.0, null, null));
var_dump(clamp(0.0, null, false));
try {
    var_dump(clamp(INF, false, NAN));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(clamp(NAN, 0, 2));
?>
--EXPECTF--
*** Testing clamp() : basic functionality 02 ***
float(0)
int(0)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
int(2)
int(10)
float(24)
float(23.45)

Deprecated: clamp(): Passing null to parameter #2 ($min) of type int|float is deprecated in %s on line %d
int(0)

Deprecated: clamp(): Passing null to parameter #2 ($min) of type int|float is deprecated in %s on line %d

Deprecated: clamp(): Passing null to parameter #3 ($max) of type int|float is deprecated in %s on line %d
int(0)

Deprecated: clamp(): Passing null to parameter #2 ($min) of type int|float is deprecated in %s on line %d
int(0)
clamp(): Argument #2 ($min) must be smaller than or equal to argument #3 ($max)
int(0)
