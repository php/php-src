--TEST--
Checks that offsetExists() does not accept a value larger than the array.
--CREDITS--
 PHPNW Test Fest 2009 - Rick Ogden
--FILE--
<?php
$ar = new SplFixedArray(3);
$ar[0] = 1;
$ar[1] = 2;
$ar[2] = 3;

var_dump($ar->offsetExists(4));
?>
--EXPECT--
bool(false)
