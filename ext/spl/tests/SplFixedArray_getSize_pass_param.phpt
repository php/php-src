--TEST--
SplFixedArray::getSize() pass a parameter when none are expected
--CREDITS--
PHPNW Testfest 2009 - Adrian Hardy
--FILE--
<?php
$fixed_array = new SplFixedArray(2);
echo "*test* ".$fixed_array->getSize(3);
?>
--EXPECTF--
Warning: SplFixedArray::getSize() expects exactly 0 parameters, 1 given in %s on line %d
*test*
