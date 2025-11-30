--TEST--
GH-20614 (SplFixedArray incorrectly handles references in deserialization)
--FILE--
<?php

$fa = new SplFixedArray(0);
$nr = 1;
$array = [&$nr];
$fa->__unserialize($array);
var_dump($fa);
unset($fa[0]);
var_dump($fa);

?>
--EXPECT--
object(SplFixedArray)#1 (1) {
  [0]=>
  int(1)
}
object(SplFixedArray)#1 (1) {
  [0]=>
  NULL
}
