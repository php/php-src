--TEST--
array_intersect_key(): Basic Test
--FILE--
<?php
/*
* Function is implemented in ext/standard/array.c
*/
$array1 = array('blue' => 1, 'red' => 2, 'green' => 3, 'purple' => 4);
$array2 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan' => 8);
var_dump(array_intersect_key($array1, $array2));
?>
--EXPECT--
array(2) {
  ["blue"]=>
  int(1)
  ["green"]=>
  int(3)
}
