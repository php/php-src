--TEST--
Test decoct() -  error conditions
--FILE--
<?php
/* Prototype  : string decbin  ( int $number  )
 * Description: Decimal to binary.
 * Source code: ext/standard/math.c
 */

echo "*** Testing decoct() :  error conditions ***\n";

echo "Incorrect number of arguments\n";
decoct();
decoct(23,2,true);

?>
===Done===
--EXPECTF--
*** Testing decoct() :  error conditions ***
Incorrect number of arguments

Warning: Wrong parameter count for decoct() in %s on line %d

Warning: Wrong parameter count for decoct() in %s on line %d
===Done===
