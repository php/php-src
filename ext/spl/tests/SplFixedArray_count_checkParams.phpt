--TEST--
Makes sure that an integer cannot be passed into the count() method of the splFixedArray.
--CREDITS--
PHPNW Test Fest 2009 - Rick Ogden
--FILE--
<?php
$ar = new SplFixedArray(3);
$ar[0] = 1;
$ar[1] = 2;
$ar[2] = 3;

echo $ar->count(3);
?>
--EXPECTF--
Warning: SplFixedArray::count() expects exactly 0 parameters, 1 given in %s on line %d
