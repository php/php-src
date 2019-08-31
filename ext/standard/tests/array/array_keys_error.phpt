--TEST--
Test array_keys() function (error conditions)
--FILE--
<?php

echo "\n*** Testing error conditions ***\n";
try {
    var_dump(array_keys(new stdclass));  // object
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(array_keys(array(1,2,3, new stdClass => array())));  // (W)illegal offset

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***
array_keys() expects parameter 1 to be array, object given

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
