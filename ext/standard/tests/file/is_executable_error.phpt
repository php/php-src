--TEST--
Test is_executable() function: error conditions
--FILE--
<?php
/* Prototype: bool is_executable ( string $filename );
   Description: Tells whether the filename is executable
*/

echo "\n*** Testing is_exceutable() on non-existent directory ***\n";
var_dump( is_executable(__DIR__."/is_executable") );

echo "Done\n";
--EXPECTF--
*** Testing is_exceutable() on non-existent directory ***
bool(false)
Done
