--TEST--
Test clearstatcache() function: error conditions(bug#42072)
--FILE--
<?php
/*
   Prototype: void clearstatcache (void);
   Description: clears files status cache
*/

echo "*** Testing clearstatcache() function: error conditions ***\n";
var_dump( clearstatcache("file") );  //No.of args more than expected
echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing clearstatcache() function: error conditions ***
Warning: clearstatcache() expects 0 parameter, 1 given in %s on line %d
NULL
*** Done ***
--UEXPECTF--
*** Testing clearstatcache() function: error conditions ***
Warning: clearstatcache() expects 0 parameter, 1 given in %s on line %d
NULL
*** Done ***
