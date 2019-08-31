--TEST--
Appending to an array via unpack may fail
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64bit only"); ?>
--FILE--
<?php

$arr = [1, 2, 3];
var_dump([PHP_INT_MAX-1 => 0, ...$arr]);

var_dump([PHP_INT_MAX-1 => 0, ...[1, 2, 3]]);

const ARR = [1, 2, 3];
const ARR2 = [PHP_INT_MAX-1 => 0, ...ARR];
var_dump(ARR2);

?>
--EXPECTF--
Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
array(2) {
  [9223372036854775806]=>
  int(0)
  [9223372036854775807]=>
  int(1)
}

Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
array(2) {
  [9223372036854775806]=>
  int(0)
  [9223372036854775807]=>
  int(1)
}

Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
array(2) {
  [9223372036854775806]=>
  int(0)
  [9223372036854775807]=>
  int(1)
}
