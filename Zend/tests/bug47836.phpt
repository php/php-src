--TEST--
Bug #47836 (array operator [] inconsistency when the array has PHP_INT_MAX index value)
--FILE--
<?php

$arr[PHP_INT_MAX] = 1;
try {
    $arr[] = 2;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($arr);
?>
--EXPECTF--
Cannot add element to the array as the next element is already occupied
array(1) {
  [%d]=>
  int(1)
}
