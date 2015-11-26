--TEST--
shrink a full array of integers
--CREDITS--
PHPNW Testfest 2009 - Lorna Mitchell
--FILE--
<?php
$array = new SplFixedArray(5);
$array[0] = 1;
$array[1] = 1;
$array[2] = 1;
$array[3] = 1;
$array[4] = 1;

$array->setSize(4);
var_dump($array);

?>
--EXPECT--
object(SplFixedArray)#1 (4) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(1)
}
