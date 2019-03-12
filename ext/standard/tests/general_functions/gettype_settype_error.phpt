--TEST--
Test gettype() & settype() functions : error conditions
--FILE--
<?php
/* Prototype: string gettype ( mixed $var );
   Description: Returns the type of the PHP variable var

   Prototype: bool settype ( mixed &$var, string $type );
   Description: Set the type of variable var to type
*/

/* Test different error conditions of settype() and gettype() functions */

echo "**** Testing gettype() and settype() functions ****\n";

echo "\n*** Testing settype(): error conditions ***\n";

// passing an invalid type to set
var_dump( settype( $var, "unknown" ) );

echo "Done\n";
?>
--EXPECTF--
**** Testing gettype() and settype() functions ****

*** Testing settype(): error conditions ***

Warning: settype(): Invalid type in %s on line %d
bool(false)
Done
