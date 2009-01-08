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

Warning: decoct() expects exactly 1 parameter, 0 given in %s on line %d

Warning: decoct() expects exactly 1 parameter, 3 given in %s on line %d
===Done===
