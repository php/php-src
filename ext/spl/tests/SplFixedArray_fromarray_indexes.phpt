--TEST--
Create a SplFixedArray from an array using the fromArray() function use the default behaviour of preserve the indexes.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$array = SplFixedArray::fromArray(array(1 => 1, 
										2 => '2',
										3 => false));
var_dump($array);
?>
--EXPECTF--
object(SplFixedArray)#1 (4) {
  [0]=>
  NULL
  [1]=>
  int(1)
  [2]=>
  %string|unicode%(1) "2"
  [3]=>
  bool(false)
}
