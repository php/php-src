--TEST--
__compareTo: Supported by array functions with equality semantics (searching)
--FILE--
<?php
include("setup.inc");

$a = new Comparable(1);
$b = new Comparable(2);
$c = new Comparable(3);

$array = [$a, $b, $c];

var_dump(in_array($a, $array));
var_dump(in_array($b, $array));
var_dump(in_array($c, $array));

/* Yes, because __compareTo returned 0 */
var_dump(in_array(new Comparable(1), $array)); 
var_dump(in_array(new Comparable(2), $array)); 
var_dump(in_array(new Comparable(3), $array)); 

/* Yes, because __compareTo returned 0 and we haven't implemented __equals */
var_dump(in_array(1, $array)); 
var_dump(in_array(2, $array));
var_dump(in_array(3, $array));

var_dump(in_array(new Comparable(4), $array)); // no
var_dump(in_array(new Comparable(5), $array)); // no

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
