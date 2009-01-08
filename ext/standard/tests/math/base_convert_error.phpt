--TEST--
Test base_convert() function :  error conditions - incorrect input
--FILE--
<?php
/* Prototype  : string base_convert  ( string $number  , int $frombase  , int $tobase  )
 * Description: Convert a number between arbitrary bases.
 * Source code: ext/standard/math.c
 */

echo "*** Testing base_convert() : error conditions ***\n";

// get a class
class classA
{
}

echo "Incorrect number of arguments\n";
base_convert();
base_convert(35);
base_convert(35,2);
base_convert(1234, 1, 10);
base_convert(1234, 10, 37);

echo "Incorrect input\n";
base_convert(new classA(), 8, 10);

?>
--EXPECTF--
*** Testing base_convert() : error conditions ***
Incorrect number of arguments

Warning: Wrong parameter count for base_convert() in %s on line %d

Warning: Wrong parameter count for base_convert() in %s on line %d

Warning: Wrong parameter count for base_convert() in %s on line %d

Warning: base_convert(): Invalid `from base' (1) in %s on line %d

Warning: base_convert(): Invalid `to base' (37) in %s on line %d
Incorrect input

Catchable fatal error: Object of class classA could not be converted to string in %s on line %d
