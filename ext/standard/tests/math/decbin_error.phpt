--TEST--
Test decbin() - error conditions 
--FILE--
<?php
/* Prototype  : string decbin  ( int $number  )
 * Description: Decimal to binary.
 * Source code: ext/standard/math.c
 */

echo "*** Testing decbin() :  error conditions ***\n";

echo "Incorrect number of arguments\n";
decbin();
decbin(23,2,true);

?>
===Done===
--EXPECTF--
*** Testing decbin() :  error conditions ***
Incorrect number of arguments

Warning: decbin() expects exactly 1 parameter, 0 given in %s on line %d

Warning: decbin() expects exactly 1 parameter, 3 given in %s on line %d
===Done===
