--TEST--
Test clamp() function : basic functionality
--FILE--
<?php
echo "*** Testing clamp() : basic functionality ***\n";

var_dump(clamp(0, 1, 2));
var_dump(clamp(2, 1, 3));
var_dump(clamp(3, 1, 4));
var_dump(clamp(5, 1, 4));
var_dump(clamp(5, 1, 5));

?>
--EXPECTF--
*** Testing abs() : basic functionality ***
int(1)
int(2)
int(3)
int(4)
int(5)
