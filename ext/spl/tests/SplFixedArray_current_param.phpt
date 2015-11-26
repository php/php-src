--TEST--
SplFixedArray::current() with a parameter. *BUG*
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( 3 );

$array[0] = "Hello";
$array[1] = "world";
$array[2] = "elePHPant";

foreach ( $array as $value ) {
	echo $array->current( array("this","should","not","execute") );
}

?>
--EXPECTF--
Warning: SplFixedArray::current() expects exactly 0 parameters, 1 given in %s on line %d

Warning: SplFixedArray::current() expects exactly 0 parameters, 1 given in %s on line %d

Warning: SplFixedArray::current() expects exactly 0 parameters, 1 given in %s on line %d
