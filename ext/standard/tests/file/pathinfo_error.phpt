--TEST--
Test pathinfo() function: error conditions
--FILE--
<?php
/* Prototype: mixed pathinfo ( string $path [, int $options] );
   Description: Returns information about a file path
*/

echo "*** Testing pathinfo() for error conditions ***\n";
/* unexpected no. of arguments */
var_dump( pathinfo() );  /* args < expected */
var_dump( pathinfo("/home/1.html", 1, 3) );  /* args > expected */

echo "Done\n";
?>
--EXPECTF--
*** Testing pathinfo() for error conditions ***

Warning: pathinfo() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: pathinfo() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
