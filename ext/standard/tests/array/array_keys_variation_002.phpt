--TEST--
Test array_keys() function (variation - 2)
--FILE--
<?php

echo "\n*** Testing array_keys() on an array created on the fly ***\n";
var_dump(array_keys(array("a" => 1, "b" => 2, "c" => 3)));
var_dump(array_keys(array()));  // null array

echo "Done\n";
?>
--EXPECT--
*** Testing array_keys() on an array created on the fly ***
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(0) {
}
Done
