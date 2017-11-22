--TEST--
Bug #48854 (array_merge_recursive modifies arrays after first one)
--FILE--
<?php

$array1 = array(
       'friends' => 5,
       'children' => array(
               'dogs' => 0,
       ),
);

$array2 = array(
       'friends' => 10,
       'children' => array(
               'cats' => 5,
       ),
);

$merged = array_merge_recursive($array1, $array2);

var_dump($array1, $array2);

?>
--EXPECTF--
array(2) {
  ["friends"]=>
  int(5)
  ["children"]=>
  array(1) {
    ["dogs"]=>
    int(0)
  }
}
array(2) {
  ["friends"]=>
  int(10)
  ["children"]=>
  array(1) {
    ["cats"]=>
    int(5)
  }
}
