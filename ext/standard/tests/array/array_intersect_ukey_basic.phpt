--TEST--
array_intersect_ukey(): Basic test.
--FILE--
<?php
/*
* Function is implemented in ext/standard/array.c
*/
function key_compare_func($key1, $key2) {
    if ($key1 == $key2) return 0;
    else if ($key1 > $key2) return 1;
    else return -1;
}
$array1 = array('blue' => 1, 'red' => 2, 'green' => 3, 'purple' => 4);
$array2 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan' => 8);
var_dump(array_intersect_ukey($array1, $array2, 'key_compare_func'));
?>
--EXPECT--
array(2) {
  ["blue"]=>
  int(1)
  ["green"]=>
  int(3)
}
