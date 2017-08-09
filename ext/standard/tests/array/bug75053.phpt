--TEST--
Bug #75053 (Integer array key limitations)
--FILE--
<?php

$arr = [5076964154930102272 => 'some value'];
$arr[999999999999999999999999999999] = 'another value';
$arr[99.99] = 'another value 2';
var_dump($arr);

$arr = [
    5076964154930102272 => 'some value',
    999999999999999999999999999999 => 'another value',
    99.99 => 'another value 2'
];
var_dump($arr);

?>
--EXPECTF--
array(%s) {
  [5076964154930102272]=>
  string(%s) "some value"
  ["1.0E+30"]=>
  string(%s) "another value"
  [99]=>
  string(%s) "another value 2"
}
array(%s) {
  [5076964154930102272]=>
  string(%s) "some value"
  ["1.0E+30"]=>
  string(%s) "another value"
  [99]=>
  string(%s) "another value 2"
}
