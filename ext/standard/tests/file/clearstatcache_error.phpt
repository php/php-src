--TEST--
Test clearstatcache() function: error conditions
--FILE--
<?php
/*
   Prototype: void clearstatcache (void);
   Description: clears files status cache
*/

echo "*** Testing clearstatcache() function: error conditions --";
var_dump( clearstatcache("file") );  //No.of args more than expected
echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing clearstatcache() function: error conditions --
Warning: Wrong parameter count for clearstatcache() in %s on line %d
NULL
*** Done ***
