--TEST--
Test array_values() function (errors)
--INI--
precision=14
--FILE--
<?php

echo "\n*** Testing error conditions ***\n";
/* Invalid number of args */
var_dump( array_values() );  // Zero arguments
var_dump( array_values(array(1,2,3), "") );  // No. of args > expected
/* Invalid types */
var_dump( array_values("") );  // Empty string
var_dump( array_values(100) );  // Integer
var_dump( array_values(new stdclass) );  // object

echo "Done\n";
?>
--EXPECTF--	
*** Testing error conditions ***

Warning: array_values() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: array_values() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: array_values() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_values() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: array_values() expects parameter 1 to be array, object given in %s on line %d
NULL
Done
