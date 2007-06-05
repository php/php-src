--TEST--
Test is_readable() function: error conditions
--FILE--
<?php
/* Prototype: bool is_readable ( string $filename );
   Description: Tells whether the filename is readable
*/

echo "*** Testing is_readable(): error conditions ***\n";
var_dump( is_readable() );  // args < expected
var_dump( is_readable(1, 2) );  // args > expected

echo "\n*** Testing is_readable() on non-existent file ***\n";
var_dump( is_readable(dirname(__FILE__)."/is_readable.tmp") );

echo "Done\n";
?>
--EXPECTF--
*** Testing is_readable(): error conditions ***

Warning: Wrong parameter count for is_readable() in %s on line %d
NULL

Warning: Wrong parameter count for is_readable() in %s on line %d
NULL

*** Testing is_readable() on non-existent file ***
bool(false)
Done
