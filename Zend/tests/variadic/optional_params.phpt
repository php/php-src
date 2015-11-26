--TEST--
Optional parameter before variadic parameter
--FILE--
<?php

function fn($reqParam, $optParam = null, ...$params) {
    var_dump($reqParam, $optParam, $params);
}
 
fn(1);
fn(1, 2);
fn(1, 2, 3);
fn(1, 2, 3, 4);
fn(1, 2, 3, 4, 5);

?>
--EXPECT--
int(1)
NULL
array(0) {
}
int(1)
int(2)
array(0) {
}
int(1)
int(2)
array(1) {
  [0]=>
  int(3)
}
int(1)
int(2)
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
int(1)
int(2)
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(4)
  [2]=>
  int(5)
}
