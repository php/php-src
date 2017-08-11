--TEST--
Bug #75053 (Integer array key limitations)
--FILE--
<?php
$arr = [5076964154930102272 => 'some value'];
$arr[999999999999999999999999999999] = 'another value';
$arr[-999999999999999999999999999999] = 'another value 2';
$arr[99.99] = 'another value 3';
var_dump($arr);
$arr = [
    5076964154930102272 => 'some value',
    999999999999999999999999999999 => 'another value',
    -999999999999999999999999999999 => 'another value 2',
    99.99 => 'another value 3'
];
var_dump($arr);

?>
--EXPECTF--
Warning: Array index overflows the maximum integer value in %s on line %s

Warning: Array index overflows the minimum integer value in %s on line %s

Warning: Array index overflows the maximum integer value in %s on line %s

Warning: Array index overflows the minimum integer value in %s on line %s
array(%s) {
  [5076964154930102272]=>
  string(%s) "another value"
  [-5076964154930102272]=>
  string(%s) "another value 2"
  [99]=>
  string(%s) "another value 3"
}
array(%s) {
  [5076964154930102272]=>
  string(%s) "another value"
  [-5076964154930102272]=>
  string(%s) "another value 2"
  [99]=>
  string(%s) "another value 3"
}
