--TEST--
var_dump float test
--INI--
precision=12
--FILE--
<?php
// this checks f,g,G conversion for snprintf/spprintf
var_dump(array(ini_get('precision'),.012,-.012,.12,-.12,1.2,-1.2,12.,-12.,0.000123,.0000123,123456789012.0,1234567890123.0,12345678901234567890.0));
?>
--EXPECT--
array(14) {
  [0]=>
  string(2) "12"
  [1]=>
  float(0.012)
  [2]=>
  float(-0.012)
  [3]=>
  float(0.12)
  [4]=>
  float(-0.12)
  [5]=>
  float(1.2)
  [6]=>
  float(-1.2)
  [7]=>
  float(12)
  [8]=>
  float(-12)
  [9]=>
  float(0.000123)
  [10]=>
  float(1.23E-5)
  [11]=>
  float(123456789012)
  [12]=>
  float(1.23456789012E+12)
  [13]=>
  float(1.23456789012E+19)
}