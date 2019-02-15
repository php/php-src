--TEST--
Test convert_uudecode() function : error conditions
--FILE--
<?php

/* Prototype  : string convert_uudecode  ( string $data  )
 * Description: Decode a uuencoded string
 * Source code: ext/standard/uuencode.c
*/

echo "*** Testing convert_uudecode() : error conditions ***\n";

echo "\n-- Testing convert_uudecode() function with no arguments --\n";
var_dump( convert_uudecode() );

echo "\n-- Testing convert_uudecode() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( convert_uudecode(72, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing convert_uudecode() : error conditions ***

-- Testing convert_uudecode() function with no arguments --

Warning: convert_uudecode() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing convert_uudecode() function with more than expected no. of arguments --

Warning: convert_uudecode() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
===DONE===
