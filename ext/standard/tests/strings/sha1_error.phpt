--TEST--
Test sha1() function : error conditions 
--FILE--
<?php


/* Prototype: string sha1  ( string $str  [, bool $raw_output  ] )
 * Description: Calculate the sha1 hash of a string
 */
 
echo "*** Testing sha1() : error conditions ***\n";

echo "\n-- Testing sha1() function with no arguments --\n";
var_dump( sha1() );

echo "\n-- Testing sha1() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( sha1("Hello World",  true, $extra_arg) );


?>
===DONE===
--EXPECTF--
*** Testing sha1() : error conditions ***

-- Testing sha1() function with no arguments --

Warning: sha1() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing sha1() function with more than expected no. of arguments --

Warning: sha1() expects at most 2 parameters, 3 given in %s on line %d
NULL
===DONE===
