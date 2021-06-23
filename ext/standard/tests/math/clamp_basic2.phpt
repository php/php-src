--TEST--
Test clamp() function : basic functionality 02
--FILE--
<?php
echo "*** Testing clamp() : basic functionality 02 ***\n";

var_dump(clamp(0, 0.0, 2));
var_dump(clamp(0.0, 0, 2));
var_dump(clamp(0.0, NAN, 2));
var_dump(clamp(INF, NAN, 2));
var_dump(clamp(0x10, 0x01, 2));
var_dump(clamp(10, "-2.345e1", 2.345e1));
var_dump(clamp(24.0, 027, 25));
var_dump(clamp(2.345e1, "-2.345e1", 2.345e1));
var_dump(clamp(0.0, null, 2));
var_dump(clamp(0.0, null, null));
var_dump(clamp(0.0, null, false));
var_dump(clamp(INF, NAN, false));
?>
--EXPECTF--
*** Testing clamp() : basic functionality 02 ***
float(0)
int(0)
float(NAN)
int(2)
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
int(0)
