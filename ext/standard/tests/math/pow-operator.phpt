--TEST--
Various pow() tests
--FILE--
<?php

$x = 2;
$x **= 3;

var_dump( -3 ** 2 === -9);
var_dump( (-3) **2 === 9);
var_dump( 2 ** 3 ** 2 === 512);
var_dump( (2 ** 3) ** 2 === 64);
var_dump( $x === 8);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
