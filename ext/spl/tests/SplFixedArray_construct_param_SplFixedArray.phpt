--TEST--
Create an SplFixedArray using an SplFixedArray object.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$array = new SplFixedArray(new SplFixedArray(3));
var_dump($array);
?>
--EXPECTF--
Warning: SplFixedArray::__construct() expects parameter 1 to be integer, object given in %s on line %d
object(SplFixedArray)#1 (0) {
}