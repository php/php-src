--TEST--
Test range() function with null as argument.
--INI--
serialize_precision=14
--FILE--
<?php
echo "range(null, null)\n";
var_dump( range(null, null) );

echo "null with int boundary\n";
var_dump( range(null, 5) );
var_dump( range(5, null) );

echo "null with float boundary\n";
var_dump( range(null, 4.5) );
var_dump( range(4.5, null) );

echo "null with string boundary\n";
var_dump( range(null, 'e') );
var_dump( range('e', null) );

echo "Done\n";
?>
--EXPECTF--
range(null, null)

Deprecated: range(): Passing null to parameter #1 ($start) of type string|int|float is deprecated in %s on line %d

Deprecated: range(): Passing null to parameter #2 ($end) of type string|int|float is deprecated in %s on line %d
array(1) {
  [0]=>
  int(0)
}
null with int boundary

Deprecated: range(): Passing null to parameter #1 ($start) of type string|int|float is deprecated in %s on line %d
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}

Deprecated: range(): Passing null to parameter #2 ($end) of type string|int|float is deprecated in %s on line %d
array(6) {
  [0]=>
  int(5)
  [1]=>
  int(4)
  [2]=>
  int(3)
  [3]=>
  int(2)
  [4]=>
  int(1)
  [5]=>
  int(0)
}
null with float boundary

Deprecated: range(): Passing null to parameter #1 ($start) of type string|int|float is deprecated in %s on line %d
array(5) {
  [0]=>
  float(0)
  [1]=>
  float(1)
  [2]=>
  float(2)
  [3]=>
  float(3)
  [4]=>
  float(4)
}

Deprecated: range(): Passing null to parameter #2 ($end) of type string|int|float is deprecated in %s on line %d
array(5) {
  [0]=>
  float(4.5)
  [1]=>
  float(3.5)
  [2]=>
  float(2.5)
  [3]=>
  float(1.5)
  [4]=>
  float(0.5)
}
null with string boundary

Deprecated: range(): Passing null to parameter #1 ($start) of type string|int|float is deprecated in %s on line %d

Warning: range(): Argument #1 ($start) must be a single byte string if argument #2 ($end) is a single byte string, argument #2 ($end) converted to 0 in %s on line %d
array(1) {
  [0]=>
  int(0)
}

Deprecated: range(): Passing null to parameter #2 ($end) of type string|int|float is deprecated in %s on line %d

Warning: range(): Argument #2 ($end) must be a single byte string if argument #1 ($start) is a single byte string, argument #1 ($start) converted to 0 in %s on line %d
array(1) {
  [0]=>
  int(0)
}
Done
