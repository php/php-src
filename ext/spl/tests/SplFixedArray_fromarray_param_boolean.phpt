--TEST--
Tries to create a SplFixedArray using a boolean value.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$array = SplFixedArray::fromArray(true);
?>
--EXPECTF--
Warning: SplFixedArray::fromArray() expects parameter 1 to be array, boolean given in %s on line %d