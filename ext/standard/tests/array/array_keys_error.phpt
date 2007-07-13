--TEST--
Test array_keys() function (error conditions)
--FILE--
<?php

echo "\n*** Testing error conditions ***";
var_dump(array_keys(100));
var_dump(array_keys("string"));
var_dump(array_keys(new stdclass));  // object
var_dump(array_keys());  // Zero arguments
var_dump(array_keys(array(), "", TRUE, 100));  // args > expected
var_dump(array_keys(array(1,2,3, array() => array())));  // (W)illegal offset

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***
Warning: array_keys(): The first argument should be an array in %s on line %d
NULL

Warning: array_keys(): The first argument should be an array in %s on line %d
NULL

Warning: array_keys(): The first argument should be an array in %s on line %d
NULL

Warning: Wrong parameter count for array_keys() in %s on line %d
NULL

Warning: Wrong parameter count for array_keys() in %s on line %d
NULL

Warning: Illegal offset type in %s on line %d
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
Done
