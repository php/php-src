--TEST--
range(): numeric string boundary inputs
--INI--
serialize_precision=14
--FILE--
<?php
echo "Increasing Range\n";
var_dump( range("1", "10") );

echo "\nDecreasing range\n";
var_dump( range("10", "1") );

echo "\nBoundaries are equal\n";
var_dump( range("5", "5") );

echo "\nPassing int step\n";
var_dump( range("1", "10", 3) );
var_dump( range("10", "1", 3) );
var_dump( range("1", "10", "3") );

echo "\nPassing float step\n";
var_dump( range("1", "2", 0.1) );
var_dump( range("2", "1", 0.1) );
var_dump( range("1", "2", "0.1") );

echo "Done\n";
?>
--EXPECT--
Increasing Range
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

Decreasing range
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

Boundaries are equal
array(1) {
  [0]=>
  string(1) "5"
}

Passing int step
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  int(7)
  [3]=>
  int(10)
}
array(4) {
  [0]=>
  int(10)
  [1]=>
  int(7)
  [2]=>
  int(4)
  [3]=>
  int(1)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  int(7)
  [3]=>
  int(10)
}

Passing float step
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
Done
