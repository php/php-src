--TEST--
Tries to create a SplFixedArray using the fromArray() function and a multi dimentional array.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$array = SplFixedArray::fromArray(array(array('1')));
var_dump($array);
?>
--EXPECTF--
object(SplFixedArray)#1 (1) {
  [0]=>
  array(1) {
    [0]=>
    %string|unicode%(1) "1"
  }
}
