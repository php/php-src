--TEST--
Test convert_uuencode() function : error conditions
--FILE--
<?php

/* Prototype  : string convert_uuencode  ( string $data  )
 * Description: Uuencode a string
 * Source code: ext/standard/uuencode.c
*/

echo "*** Testing convert_uuencode() : error conditions ***\n";

echo "\n-- Testing chconvert_uuencoder() function with no arguments --\n";
var_dump( convert_uuencode() );

echo "\n-- Testing convert_uuencode() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( convert_uuencode(72, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing convert_uuencode() : error conditions ***

-- Testing chconvert_uuencoder() function with no arguments --

Warning: convert_uuencode() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing convert_uuencode() function with more than expected no. of arguments --

Warning: convert_uuencode() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
===DONE===
