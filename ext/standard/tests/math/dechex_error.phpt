--TEST--
Test dechex() - wrong params dechex()
--FILE--
<?php
/* Prototype  : string dechex  ( int $number  )
 * Description: Returns a string containing a hexadecimal representation of the given number  argument.
 * Source code: ext/standard/math.c
 */

echo "*** Testing dechex() : error conditions ***\n";

echo "\nIncorrect number of arguments\n"; 
dechex();
dechex(23,2,true);

?>
===Done===
--EXPECTF--
*** Testing dechex() : error conditions ***

Incorrect number of arguments

Warning: Wrong parameter count for dechex() in %s on line %d

Warning: Wrong parameter count for dechex() in %s on line %d
===Done===