--TEST--
SplFixedArray::key() when the array has a size higher than the amount of values specified.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( 4 );

$array[0] = "Hello";
$array[1] = "world";
$array[2] = "elePHPant";

foreach ( $array as $value ) {
	echo $array->key( );
}

?>
--EXPECT--
0123
