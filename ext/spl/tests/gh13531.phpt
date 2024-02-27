--TEST--
GH-13531 (Unable to resize SplfixedArray after being unserialized in PHP 8.2.15)
--FILE--
<?php

$array = new SplFixedArray(5);
$array[4] = 1;
$serialized = serialize($array);
$unserialized = unserialize($serialized);
$unserialized->setSize(6);
var_dump($unserialized);

?>
--EXPECT--
object(SplFixedArray)#2 (6) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  int(1)
  [5]=>
  NULL
}
