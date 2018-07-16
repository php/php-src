--TEST--
__equals: Called by in_array which has equality semantics (searching)
--FILE--
<?php
include("Equatable.inc");

$a = new Equatable(1);
$b = new Equatable(2);
$c = new Equatable(3);

$array = [$a, $b, $c];

var_dump(in_array($a, $array, $strict = false)); 
var_dump(in_array($b, $array, $strict = false)); 
var_dump(in_array($c, $array, $strict = false)); 

var_dump(in_array($a, $array, $strict = true)); 
var_dump(in_array($b, $array, $strict = true)); 
var_dump(in_array($c, $array, $strict = true)); 

/* Found, because Equatable::__equals returned TRUE */
var_dump(in_array(new Equatable(1), $array, $strict = false)); 
var_dump(in_array(new Equatable(2), $array, $strict = false)); 
var_dump(in_array(new Equatable(3), $array, $strict = false)); 

/* Not found, because not the same instance */
var_dump(in_array(new Equatable(1), $array, $strict = true));
var_dump(in_array(new Equatable(2), $array, $strict = true));
var_dump(in_array(new Equatable(3), $array, $strict = true));

/* Found, because Equatable::__equals returned TRUE */
var_dump(in_array(1, $array, $strict = false));
var_dump(in_array(2, $array, $strict = false));
var_dump(in_array(3, $array, $strict = false));

/* Not found, because strict comparison doesn't call __equals */
var_dump(in_array(1, $array, $strict = true)); 
var_dump(in_array(2, $array, $strict = true)); 
var_dump(in_array(3, $array, $strict = true)); 

/* Not found */
var_dump(in_array(new Equatable(4), $array, $strict = false)); 
var_dump(in_array(new Equatable(5), $array, $strict = false)); 

var_dump(in_array(new Equatable(4), $array, $strict = true)); 
var_dump(in_array(new Equatable(5), $array, $strict = true)); 

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
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
