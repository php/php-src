--TEST--
__equals: Supported by array functions with equality semantics (searching)
--FILE--
<?php
include("setup.inc");

$a = new Equatable(1);
$b = new Equatable(2);
$c = new Equatable(3);

$array = [$a, $b, $c];

/* Yes, because __equals returned true */
var_dump(in_array(new Equatable(1), $array)); 
var_dump(in_array(new Equatable(2), $array)); 
var_dump(in_array(new Equatable(3), $array)); 

/* No, because __equals does an instanceof check */
var_dump(in_array(1, $array)); 
var_dump(in_array(2, $array));
var_dump(in_array(3, $array));

var_dump(in_array(new Equatable(4), $array));
var_dump(in_array(new Equatable(5), $array));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
