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

$arr = [5076964154930102272 => 'some value'];
$key1 = 999999999999999999999999999999;
$key2 = -999999999999999999999999999999;
$arr[$key1] = 'another value';
$arr[$key2] = 'another value 2';
$arr[99.99] = 'another value 3';

var_dump($arr);
var_dump($key1);
var_dump($key2);

?>
--EXPECTF--
array(%s) {
  [5076964154930102272]=>
  string(%s) "some value"
  ["1.0E+30"]=>
  string(%s) "another value"
  ["-1.0E+30"]=>
  string(%s) "another value 2"
  [99]=>
  string(%s) "another value 3"
}
array(%s) {
  [5076964154930102272]=>
  string(%s) "some value"
  ["1.0E+30"]=>
  string(%s) "another value"
  ["-1.0E+30"]=>
  string(%s) "another value 2"
  [99]=>
  string(%s) "another value 3"
}
array(%s) {
  [5076964154930102272]=>
  string(%s) "some value"
  ["1.0E+30"]=>
  string(%s) "another value"
  ["-1.0E+30"]=>
  string(%s) "another value 2"
  [99]=>
  string(%s) "another value 3"
}
float(1.0E+30)
float(-1.0E+30)
