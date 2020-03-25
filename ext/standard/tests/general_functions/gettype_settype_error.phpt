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
try {
    settype( $var, "unknown" );
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
**** Testing gettype() and settype() functions ****

*** Testing settype(): error conditions ***
settype(): Argument #2 ($type) must be a valid type
Done
