--TEST--
Test array_values() function (variation)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--INI--
precision=14
--FILE--
<?php

echo "\n*** Testing array_values() with resource type ***\n";
$resource1 = fopen(__FILE__, "r");  // Creating a file resource
$resource2 = opendir(".");  // Creating a dir resource

/* creating an array with resources as elements */
$arr_resource = array( "a" => $resource1, "b" => $resource2);
var_dump( array_values($arr_resource) );

echo "\n*** Testing array_values() with range checking ***\n";
$arr_range = array(
  2147483647,
  2147483648,
  -2147483647,
  -2147483648,
  -0,
  0,
  -2147483649
);
var_dump( array_values($arr_range) );

echo "\n*** Testing array_values() on an array created on the fly ***\n";
var_dump( array_values(array(1,2,3)) );
var_dump( array_values(array()) );  // null array

echo "Done\n";
?>
--EXPECTF--
*** Testing array_values() with resource type ***
array(2) {
  [0]=>
  resource(%d) of type (stream)
  [1]=>
  resource(%d) of type (stream)
}

*** Testing array_values() with range checking ***
array(7) {
  [0]=>
  int(2147483647)
  [1]=>
  float(2147483648)
  [2]=>
  int(-2147483647)
  [3]=>
  float(-2147483648)
  [4]=>
  int(0)
  [5]=>
  int(0)
  [6]=>
  float(-2147483649)
}

*** Testing array_values() on an array created on the fly ***
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(0) {
}
Done
