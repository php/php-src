--TEST--
range(): mixed numeric types boundary inputs
--INI--
serialize_precision=14
--FILE--
<?php
var_dump( range(1, "4") );
var_dump( range("1", 4) );
var_dump( range(1.5, "4.5") );
var_dump( range("1.5", 4.5) );
var_dump( range(9, "12") );
var_dump( range("9", 12) );
?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
array(4) {
  [0]=>
  float(1.5)
  [1]=>
  float(2.5)
  [2]=>
  float(3.5)
  [3]=>
  float(4.5)
}
array(4) {
  [0]=>
  float(1.5)
  [1]=>
  float(2.5)
  [2]=>
  float(3.5)
  [3]=>
  float(4.5)
}
array(4) {
  [0]=>
  int(9)
  [1]=>
  int(10)
  [2]=>
  int(11)
  [3]=>
  int(12)
}
array(4) {
  [0]=>
  int(9)
  [1]=>
  int(10)
  [2]=>
  int(11)
  [3]=>
  int(12)
}
