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

echo "\n*** Testing gettype(): error conditions ***\n";
//Zero arguments
var_dump( gettype() );
// args more than expected 
var_dump( gettype( "1", "2" ) );

echo "\n*** Testing settype(): error conditions ***\n";
//Zero arguments
var_dump( settype() );

// args more than expected 
$var = 10.5;
var_dump( settype( $var, $var, "int" ) );

// passing an invalid type to set
var_dump( settype( $var, "unknown" ) );

echo "Done\n";
?>
--EXPECTF--	
**** Testing gettype() and settype() functions ****

*** Testing gettype(): error conditions ***

Warning: gettype() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: gettype() expects exactly 1 parameter, 2 given in %s on line %d
NULL

*** Testing settype(): error conditions ***

Warning: settype() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: settype() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: settype(): Invalid type in %s on line %d
bool(false)
Done
