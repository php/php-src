--TEST--
SplFixedArray::rewind() with a parameter. *BUG*
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( 4 );

$array[0] = "Hello";
$array[1] = "world";
$array[2] = "elePHPant";

$array->rewind( "invalid" );

?>
--EXPECTF--
Warning: SplFixedArray::rewind() expects exactly 0 parameters, 1 given in %s on line %d
