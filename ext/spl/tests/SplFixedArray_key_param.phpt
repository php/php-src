--TEST--
SplFixedArray::key() with a parameter passed. This is a bug and an error should be called.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( 3 );

$array[0] = "Hello";
$array[1] = "world";
$array[2] = "elePHPant";

foreach ( $array as $value ) {
	echo $array->key( array("this","should","not","execute") );
}

?>
--EXPECTF--
Warning: SplFixedArray::key() expects exactly 0 parameters, 1 given in %s on line %d

Warning: SplFixedArray::key() expects exactly 0 parameters, 1 given in %s on line %d

Warning: SplFixedArray::key() expects exactly 0 parameters, 1 given in %s on line %d
