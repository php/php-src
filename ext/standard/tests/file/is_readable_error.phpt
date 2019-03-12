--TEST--
Test is_readable() function: error conditions
--FILE--
<?php
/* Prototype: bool is_readable ( string $filename );
   Description: Tells whether the filename is readable
*/

echo "\n*** Testing is_readable() on non-existent file ***\n";
var_dump( is_readable(dirname(__FILE__)."/is_readable.tmp") );

echo "Done\n";
?>
--EXPECTF--
*** Testing is_readable() on non-existent file ***
bool(false)
Done
