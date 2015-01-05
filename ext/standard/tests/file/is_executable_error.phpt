--TEST--
Test is_executable() function: error conditions
--FILE--
<?php
/* Prototype: bool is_executable ( string $filename );
   Description: Tells whether the filename is executable
*/

echo "*** Testing is_executable(): error conditions ***\n";
var_dump( is_executable() );  // args < expected no of arguments

var_dump( is_executable(1, 2) );  // args > expected no. of arguments

echo "\n*** Testing is_exceutable() on non-existent directory ***\n";
var_dump( is_executable(dirname(__FILE__)."/is_executable") );

echo "Done\n";
--EXPECTF--
*** Testing is_executable(): error conditions ***

Warning: is_executable() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: is_executable() expects exactly 1 parameter, 2 given in %s on line %d
NULL

*** Testing is_exceutable() on non-existent directory ***
bool(false)
Done
