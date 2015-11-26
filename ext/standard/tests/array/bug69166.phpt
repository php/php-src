--TEST--
Fixed #69166 (Assigning array_values() to array does not reset key counter)
--FILE--
<?php

$array = [0];
$ar = array_values($array);
$ar[] = 1;
var_dump($ar);
?>
--EXPECT--
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
