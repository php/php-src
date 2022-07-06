--TEST--
Bug #78409: Segfault when creating instance of ArrayIterator without constructor
--FILE--
<?php

$a = new ArrayObject;
$u = [
    0,
    [],
    [],
];
$a->__unserialize($u);
var_dump($u);

?>
--EXPECT--
array(3) {
  [0]=>
  int(0)
  [1]=>
  array(0) {
  }
  [2]=>
  array(0) {
  }
}
