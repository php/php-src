--TEST--
SplFixedArray::setSize() with a float param
--CREDITS--
PHPNW Testfest 2009 - Adrian Hardy
--FILE--
<?php
$fixed_array = new SplFixedArray(2);
$fixed_array->setSize(3.14159);
var_dump($fixed_array);
?>
--EXPECT--
object(SplFixedArray)#1 (3) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
}
