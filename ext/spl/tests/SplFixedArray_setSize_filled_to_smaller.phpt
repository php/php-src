--TEST--
Create array, fills it with and resizes it to lower value.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$array = new SplFixedArray(5);
$array[0] = 1;
$array[1] = 1;
$array[2] = 1;
$array[3] = 1;
$array[4] = 1;
$array->setSize(2);
var_dump($array);
?>
--EXPECT--
object(SplFixedArray)#1 (2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}