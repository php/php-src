--TEST--
Test range() function where boundary are string digits and step is a float
--INI--
serialize_precision=14
--FILE--
<?php
var_dump( range("1", "2", 0.1) );
?>
--EXPECT--
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
