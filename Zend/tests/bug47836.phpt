--TEST--
Bug #47836 (array operator [] inconsistency when the array has PHP_INT_MAX index value)
--FILE--
<?php

$arr[PHP_INT_MAX] = 1;
$arr[] = 2;

var_dump($arr);
?>
--EXPECTF--
Warning: Cannot add element to the array as the next element is already occupied in %s on line 4
array(1) {
  [%d]=>
  int(1)
}
