--TEST--
Test range() function (variation-2)
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

echo "\n*** Testing max/outof range values ***\n";
/*var_dump( range("a", "z", 255) );
var_dump( range("z", "a", 255) ); */
var_dump( range(2147483645, 2147483646) );
var_dump( range(2147483646, 2147483648) );
var_dump( range(-2147483647, -2147483646) );
var_dump( range(-2147483648, -2147483647) );
var_dump( range(-2147483649, -2147483647) );

echo "\nDone";
?>
--EXPECT--
*** Testing max/outof range values ***
array(2) {
  [0]=>
  int(2147483645)
  [1]=>
  int(2147483646)
}
array(3) {
  [0]=>
  float(2147483646)
  [1]=>
  float(2147483647)
  [2]=>
  float(2147483648)
}
array(2) {
  [0]=>
  int(-2147483647)
  [1]=>
  int(-2147483646)
}
array(2) {
  [0]=>
  float(-2147483648)
  [1]=>
  float(-2147483647)
}
array(3) {
  [0]=>
  float(-2147483649)
  [1]=>
  float(-2147483648)
  [2]=>
  float(-2147483647)
}

Done
