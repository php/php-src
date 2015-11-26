--TEST--
Creates array, uses the count function to get the size of the array, but passes a parameter.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$array = new SplFixedArray(5);
echo $array->count(3);
?>
--EXPECTF--
Warning: SplFixedArray::count() expects exactly 0 parameters, 1 given in %s on line %d