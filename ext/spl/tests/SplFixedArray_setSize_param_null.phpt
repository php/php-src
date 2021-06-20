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
--EXPECTF--
Deprecated: SplFixedArray::setSize(): Passing null to parameter #1 ($size) of type int is deprecated in %s on line %d
object(SplFixedArray)#1 (0) {
}
