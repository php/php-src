--TEST--
Test ord() function : error conditions
--FILE--
<?php

/* Prototype  : int ord  ( string $string  )
 * Description: Return ASCII value of character
 * Source code: ext/standard/string.c
*/

echo "*** Testing ord() : error conditions ***\n";

echo "\n-- Testing ord() function with no arguments --\n";
var_dump( ord() );

echo "\n-- Testing ord() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( ord(72, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing ord() : error conditions ***

-- Testing ord() function with no arguments --

Warning: ord() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ord() function with more than expected no. of arguments --

Warning: ord() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
