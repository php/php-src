--TEST--
Test clearstatcache() function: error conditions
--FILE--
<?php
/*
   Prototype: void clearstatcache ([bool clear_realpath_cache[, filename]]);
   Description: clears files status cache
*/

echo "*** Testing clearstatcache() function: error conditions ***\n";
var_dump( clearstatcache(0, "/foo/bar", 1) );  //No.of args more than expected
echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing clearstatcache() function: error conditions ***

Warning: clearstatcache() expects at most 2 parameters, 3 given in %s on line %d
NULL
*** Done ***
