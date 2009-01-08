--TEST--
Test bindec() function :  error conditions - incorrect input
--FILE--
<?php
/* Prototype  : number bindec  ( string $binary_string  )
 * Description: Returns the decimal equivalent of the binary number represented by the binary_string  argument.
 * Source code: ext/standard/math.c
 */

/*
 * Pass incorrect input to bindec() to test behaviour
 */
 
echo "*** Testing bindec() : error conditions ***\n";

// get a class
class classA
{
}

echo "Incorrect number of arguments\n";
bindec();
bindec('01010101111',true);

echo "Incorrect input\n";
bindec(new classA());
?>
--EXPECTF--
*** Testing bindec() : error conditions ***
Incorrect number of arguments

Warning: Wrong parameter count for bindec() in %s on line %d

Warning: Wrong parameter count for bindec() in %s on line %d
Incorrect input

Catchable fatal error: Object of class classA could not be converted to string in %s on line %d
