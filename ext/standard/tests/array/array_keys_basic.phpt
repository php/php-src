--TEST--
Test array_keys() function (basic)
--FILE--
<?php

echo "*** Testing array_keys() on basic array operation ***\n";
$basic_arr = array("a" => 1, "b" => 2, 2 => 2.0, -23 => "asdasd",
                   array(1,2,3));
var_dump(array_keys($basic_arr));

echo "Done\n";
?>
--EXPECT--
*** Testing array_keys() on basic array operation ***
array(5) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  int(2)
  [3]=>
  int(-23)
  [4]=>
  int(3)
}
Done
