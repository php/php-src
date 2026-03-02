--TEST--
range(): float boundary inputs
--INI--
serialize_precision=14
--FILE--
<?php
echo "Increasing Range\n";
var_dump( range(1.5, 6.5) );

echo "\nDecreasing range\n";
var_dump( range(6.5, 1.5) );

echo "\nBoundaries are equal\n";
var_dump( range(5.5, 5.5) );

echo "\nPassing int step\n";
var_dump( range(1.5, 10.5, 3) );
var_dump( range(10.5, 1.5, 3) );
var_dump( range(1.5, 10.5, "3") );

echo "\nPassing float step\n";
var_dump( range(1.6, 2.2, 0.1) );
var_dump( range(2.2, 1.6, 0.1) );
var_dump( range(1.6, 2.2, "0.1") );

echo "Done\n";
?>
--EXPECT--
Increasing Range
array(6) {
  [0]=>
  float(1.5)
  [1]=>
  float(2.5)
  [2]=>
  float(3.5)
  [3]=>
  float(4.5)
  [4]=>
  float(5.5)
  [5]=>
  float(6.5)
}

Decreasing range
array(6) {
  [0]=>
  float(6.5)
  [1]=>
  float(5.5)
  [2]=>
  float(4.5)
  [3]=>
  float(3.5)
  [4]=>
  float(2.5)
  [5]=>
  float(1.5)
}

Boundaries are equal
array(1) {
  [0]=>
  float(5.5)
}

Passing int step
array(4) {
  [0]=>
  float(1.5)
  [1]=>
  float(4.5)
  [2]=>
  float(7.5)
  [3]=>
  float(10.5)
}
array(4) {
  [0]=>
  float(10.5)
  [1]=>
  float(7.5)
  [2]=>
  float(4.5)
  [3]=>
  float(1.5)
}
array(4) {
  [0]=>
  float(1.5)
  [1]=>
  float(4.5)
  [2]=>
  float(7.5)
  [3]=>
  float(10.5)
}

Passing float step
array(7) {
  [0]=>
  float(1.6)
  [1]=>
  float(1.7)
  [2]=>
  float(1.8)
  [3]=>
  float(1.9)
  [4]=>
  float(2)
  [5]=>
  float(2.1)
  [6]=>
  float(2.2)
}
array(7) {
  [0]=>
  float(2.2)
  [1]=>
  float(2.1)
  [2]=>
  float(2)
  [3]=>
  float(1.9)
  [4]=>
  float(1.8)
  [5]=>
  float(1.7)
  [6]=>
  float(1.6)
}
array(7) {
  [0]=>
  float(1.6)
  [1]=>
  float(1.7)
  [2]=>
  float(1.8)
  [3]=>
  float(1.9)
  [4]=>
  float(2)
  [5]=>
  float(2.1)
  [6]=>
  float(2.2)
}
Done
