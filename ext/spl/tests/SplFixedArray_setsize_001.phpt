--TEST--
SPL: FixedArray: setsize - populate array, then shrink
--CREDITS--
PHPNW TestFest2009 - Rowan Merewood <rowan@merewood.org>
--FILE--
<?php
$array = new SplFixedArray(5);
$array[0] = 'one';
$array[1] = 'two';
$array[2] = 'three';
$array[3] = 'four';
$array[4] = 'five';
$array->setSize(2);
var_dump($array);
?>
--EXPECTF--
object(SplFixedArray)#1 (2) {
  [0]=>
  %string|unicode%(3) "one"
  [1]=>
  %string|unicode%(3) "two"
}
