--TEST--
Test range() function with basic/expected string inputs
--INI--
serialize_precision=14
--FILE--
<?php

echo "Range will ignore any byte after the first one\n";
var_dump( range("AA", "BB") );

echo "Range cannot operate on an empty string\n";
var_dump( range("Z", "") ); // Both strings are cast to int, i.e. 0
var_dump( range("", "Z") ); // Both strings are cast to int, i.e. 0

echo "Mixing numeric string and character\n";
var_dump( range("1", "A") ); // The char is cast to an int, i.e. 0
var_dump( range("?", "1") ); // The char is cast to an int, i.e. 0
var_dump( range("3.5", "A") ); // The char is cast to a float, i.e. 0
var_dump( range("?", "3.5") ); // The char is cast to a float, i.e. 0

echo "Fractional step cannot be used on character ranges\n";
var_dump( range("A", "H", 2.6) ); // Because step is fractional it tries to interpret inputs as floats

echo "Done\n";
?>
--EXPECT--
Range will ignore any byte after the first one
array(2) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
}
Range cannot operate on an empty string
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
Mixing numeric string and character
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(4) {
  [0]=>
  float(3.5)
  [1]=>
  float(2.5)
  [2]=>
  float(1.5)
  [3]=>
  float(0.5)
}
array(4) {
  [0]=>
  float(0)
  [1]=>
  float(1)
  [2]=>
  float(2)
  [3]=>
  float(3)
}
Fractional step cannot be used on character ranges
array(1) {
  [0]=>
  float(0)
}
Done
