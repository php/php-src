--TEST--
Test explode() function : error conditions
--FILE--
<?php

/* Prototype  : array explode  ( string $delimiter  , string $string  [, int $limit  ] )
 * Description: Split a string by string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing explode() : error conditions ***\n";

echo "\n-- Testing explode() function with no arguments --\n";
var_dump( explode() );

echo "\n-- Testing explode() function with more than expected no. of arguments --\n";
$delimeter = " ";
$string = "piece1 piece2 piece3 piece4 piece5 piece6";
$limit = 5;
$extra_arg = 10;
var_dump( explode($delimeter, $string, $limit, $extra_arg) );

?>
===Done===
--EXPECTF--
*** Testing explode() : error conditions ***

-- Testing explode() function with no arguments --

Warning: explode() expects at least 2 parameters, 0 given in %s on line %d
NULL

-- Testing explode() function with more than expected no. of arguments --

Warning: explode() expects at most 3 parameters, 4 given in %s on line %d
NULL
===Done===