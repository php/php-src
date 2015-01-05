--TEST--
Test hexdec() - wrong params test hexdec()
--FILE--
<?php
/* Prototype  : number hexdec  ( string $hex_string  )
 * Description: Returns the decimal equivalent of the hexadecimal number represented by the hex_string  argument. 
 * Source code: ext/standard/math.c
 */

echo "*** Testing hexdec() :  error conditions ***\n";

// get a class
class classA
{
}

echo "\n-- Incorrect number of arguments --\n";
hexdec();
hexdec('0x123abc',true);

echo "\n-- Incorrect input --\n";
hexdec(new classA());

?>
--EXPECTF--
*** Testing hexdec() :  error conditions ***

-- Incorrect number of arguments --

Warning: hexdec() expects exactly 1 parameter, 0 given in %s on line %d

Warning: hexdec() expects exactly 1 parameter, 2 given in %s on line %d

-- Incorrect input --

Catchable fatal error: Object of class classA could not be converted to string in %s on line %d