--TEST--
Test range() function with unexpected string inputs
--INI--
serialize_precision=14
--FILE--
<?php

echo "Range will ignore any byte after the first one\n";
var_dump( range("AA", "BB") );

echo "Range cannot operate on an empty string\n";
var_dump( range("Z", "") ); // Both strings are cast to int, i.e. 0
var_dump( range("", "Z") ); // Both strings are cast to int, i.e. 0

echo "Mixing numeric float string and character\n";
var_dump( range("3.5", "A") ); // The char is cast to a float, i.e. 0
var_dump( range("?", "3.5") ); // The char is cast to a float, i.e. 0

echo "Fractional step cannot be used on character ranges\n";
var_dump( range("A", "H", 2.6) ); // Because step is fractional it tries to interpret inputs as floats

echo "Done\n";
?>
--EXPECTF--
Range will ignore any byte after the first one

Warning: range(): Argument #1 ($start) must be a single byte, subsequent bytes are ignored in %s on line %d

Warning: range(): Argument #2 ($end) must be a single byte, subsequent bytes are ignored in %s on line %d
array(2) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
}
Range cannot operate on an empty string

Warning: range(): Argument #2 ($end) must not be empty, casted to 0 in %s on line %d

Warning: range(): Argument #2 ($end) must be a single byte string if argument #1 ($start) is a single byte string, argument #1 ($start) converted to 0 in %s on line %d
array(1) {
  [0]=>
  int(0)
}

Warning: range(): Argument #1 ($start) must not be empty, casted to 0 in %s on line %d

Warning: range(): Argument #1 ($start) must be a single byte string if argument #2 ($end) is a single byte string, argument #2 ($end) converted to 0 in %s on line %d
array(1) {
  [0]=>
  int(0)
}
Mixing numeric float string and character

Warning: range(): Argument #1 ($start) must be a single byte string if argument #2 ($end) is a single byte string, argument #2 ($end) converted to 0 in %s on line %d
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

Warning: range(): Argument #2 ($end) must be a single byte string if argument #1 ($start) is a single byte string, argument #1 ($start) converted to 0 in %s on line %d
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

Warning: range(): Argument #3 ($step) must be of type int when generating an array of characters, inputs converted to 0 in %s on line %d
array(1) {
  [0]=>
  float(0)
}
Done
