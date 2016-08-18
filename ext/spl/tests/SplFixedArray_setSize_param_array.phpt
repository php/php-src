--TEST--
SplFixedArray::setSize() with an array parameter
--CREDITS--
PHPNW Testfest 2009 - Adrian Hardy
--FILE--
<?php
$fixed_array = new SplFixedArray(2);
$fixed_array->setSize(array());
var_dump($fixed_array);
?>
--EXPECTF--
Warning: SplFixedArray::setSize() expects parameter 1 to be integer, array given in %s on line %d
object(SplFixedArray)#1 (2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
