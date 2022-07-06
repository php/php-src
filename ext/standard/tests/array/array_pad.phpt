--TEST--
array_pad() tests
--FILE--
<?php

var_dump(array_pad(array(), 1, 0));

var_dump(array_pad(array(), 0, 0));
var_dump(array_pad(array(), -1, 0));
var_dump(array_pad(array("", -1, 2.0), 5, 0));
var_dump(array_pad(array("", -1, 2.0), 5, array()));
var_dump(array_pad(array("", -1, 2.0), 2, array()));
var_dump(array_pad(array("", -1, 2.0), -3, array()));
var_dump(array_pad(array("", -1, 2.0), -4, array()));

try {
    var_dump(array_pad(array("", -1, 2.0), 2000000, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
array(1) {
  [0]=>
  int(0)
}
array(0) {
}
array(1) {
  [0]=>
  int(0)
}
array(5) {
  [0]=>
  string(0) ""
  [1]=>
  int(-1)
  [2]=>
  float(2)
  [3]=>
  int(0)
  [4]=>
  int(0)
}
array(5) {
  [0]=>
  string(0) ""
  [1]=>
  int(-1)
  [2]=>
  float(2)
  [3]=>
  array(0) {
  }
  [4]=>
  array(0) {
  }
}
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  int(-1)
  [2]=>
  float(2)
}
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  int(-1)
  [2]=>
  float(2)
}
array(4) {
  [0]=>
  array(0) {
  }
  [1]=>
  string(0) ""
  [2]=>
  int(-1)
  [3]=>
  float(2)
}
array_pad(): Argument #2 ($length) must be less than or equal to 1048576
