--TEST--
Test range() function with basic/expected numeric inputs
--INI--
serialize_precision=14
--FILE--
<?php

echo "-- Integers as Low and High --\n";
echo "-- An array of elements from low to high --\n";
var_dump( range(1, 10) );
echo "\n-- An array of elements from high to low --\n";
var_dump( range(10, 1) );

echo "\n-- Numeric Strings as Low and High --\n";
echo "-- An array of elements from low to high --\n";
var_dump( range("1", "10") );
echo "\n-- An array of elements from high to low --\n";
var_dump( range("10", "1") );

echo "\n-- Low and High are equal --\n";
var_dump( range(5, 5) );

echo "\n-- floats as Low and High --\n";
var_dump( range(5.1, 10.1) );
var_dump( range(10.1, 5.1) );

var_dump( range("5.1", "10.1") );
var_dump( range("10.1", "5.1") );

echo "\n-- Passing step with Low and High --\n";
var_dump( range(1, 2, 0.1) );
var_dump( range(2, 1, 0.1) );

var_dump( range(1, 2, "0.1") );
var_dump( range("1", "2", 0.1) );

echo "Done\n";
?>
--EXPECT--
-- Integers as Low and High --
-- An array of elements from low to high --
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}

-- An array of elements from high to low --
array(10) {
  [0]=>
  int(10)
  [1]=>
  int(9)
  [2]=>
  int(8)
  [3]=>
  int(7)
  [4]=>
  int(6)
  [5]=>
  int(5)
  [6]=>
  int(4)
  [7]=>
  int(3)
  [8]=>
  int(2)
  [9]=>
  int(1)
}

-- Numeric Strings as Low and High --
-- An array of elements from low to high --
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}

-- An array of elements from high to low --
array(10) {
  [0]=>
  int(10)
  [1]=>
  int(9)
  [2]=>
  int(8)
  [3]=>
  int(7)
  [4]=>
  int(6)
  [5]=>
  int(5)
  [6]=>
  int(4)
  [7]=>
  int(3)
  [8]=>
  int(2)
  [9]=>
  int(1)
}

-- Low and High are equal --
array(1) {
  [0]=>
  int(5)
}

-- floats as Low and High --
array(6) {
  [0]=>
  float(5.1)
  [1]=>
  float(6.1)
  [2]=>
  float(7.1)
  [3]=>
  float(8.1)
  [4]=>
  float(9.1)
  [5]=>
  float(10.1)
}
array(6) {
  [0]=>
  float(10.1)
  [1]=>
  float(9.1)
  [2]=>
  float(8.1)
  [3]=>
  float(7.1)
  [4]=>
  float(6.1)
  [5]=>
  float(5.1)
}
array(6) {
  [0]=>
  float(5.1)
  [1]=>
  float(6.1)
  [2]=>
  float(7.1)
  [3]=>
  float(8.1)
  [4]=>
  float(9.1)
  [5]=>
  float(10.1)
}
array(6) {
  [0]=>
  float(10.1)
  [1]=>
  float(9.1)
  [2]=>
  float(8.1)
  [3]=>
  float(7.1)
  [4]=>
  float(6.1)
  [5]=>
  float(5.1)
}

-- Passing step with Low and High --
array(11) {
  [0]=>
  float(1)
  [1]=>
  float(1.1)
  [2]=>
  float(1.2)
  [3]=>
  float(1.3)
  [4]=>
  float(1.4)
  [5]=>
  float(1.5)
  [6]=>
  float(1.6)
  [7]=>
  float(1.7)
  [8]=>
  float(1.8)
  [9]=>
  float(1.9)
  [10]=>
  float(2)
}
array(11) {
  [0]=>
  float(2)
  [1]=>
  float(1.9)
  [2]=>
  float(1.8)
  [3]=>
  float(1.7)
  [4]=>
  float(1.6)
  [5]=>
  float(1.5)
  [6]=>
  float(1.4)
  [7]=>
  float(1.3)
  [8]=>
  float(1.2)
  [9]=>
  float(1.1)
  [10]=>
  float(1)
}
array(11) {
  [0]=>
  float(1)
  [1]=>
  float(1.1)
  [2]=>
  float(1.2)
  [3]=>
  float(1.3)
  [4]=>
  float(1.4)
  [5]=>
  float(1.5)
  [6]=>
  float(1.6)
  [7]=>
  float(1.7)
  [8]=>
  float(1.8)
  [9]=>
  float(1.9)
  [10]=>
  float(2)
}
array(11) {
  [0]=>
  float(1)
  [1]=>
  float(1.1)
  [2]=>
  float(1.2)
  [3]=>
  float(1.3)
  [4]=>
  float(1.4)
  [5]=>
  float(1.5)
  [6]=>
  float(1.6)
  [7]=>
  float(1.7)
  [8]=>
  float(1.8)
  [9]=>
  float(1.9)
  [10]=>
  float(2)
}
Done
