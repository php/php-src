--TEST--
SplFixedArray::setSize() with a null parameter
--CREDITS--
PHPNW Testfest 2009 - Adrian Hardy
--FILE--
<?php
$fixed_array = new SplFixedArray(2);
$fixed_array->setSize(null);
var_dump($fixed_array);
?>
--EXPECT--
object(SplFixedArray)#1 (0) {
}
