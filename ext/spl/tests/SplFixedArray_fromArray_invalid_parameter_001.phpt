--TEST--
pass an integer into fromArray()
--CREDITS--
PHPNW Testfest 2009 - Lorna Mitchell
--FILE--
<?php
echo SplFixedArray::fromArray(17954);
?>
--EXPECTF--
Warning: SplFixedArray::fromArray() expects parameter 1 to be array, int given in %s on line %d
