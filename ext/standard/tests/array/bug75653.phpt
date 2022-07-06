--TEST--
Bug #75653: array_values don't work on empty array
--FILE--
<?php

$array[] = 'data1';
unset($array[0]);
$array = array_values($array);
$array[] = 'data2';
$array[] = 'data3';
var_dump($array);

?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "data2"
  [1]=>
  string(5) "data3"
}
