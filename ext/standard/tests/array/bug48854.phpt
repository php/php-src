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
  [%u|b%"friends"]=>
  int(5)
  [%u|b%"children"]=>
  array(1) {
    [%u|b%"dogs"]=>
    int(0)
  }
}
array(2) {
  [%u|b%"friends"]=>
  int(10)
  [%u|b%"children"]=>
  array(1) {
    [%u|b%"cats"]=>
    int(5)
  }
}
