--TEST--
Test debug_zval_dump() function : error conditions
--FILE--
<?php
/* Prototype: void debug_zval_dump ( mixed $variable );
   Description: Dumps a string representation of an internal zend value 
                to output.
*/

echo "*** Testing error conditions ***\n";

/* passing zero argument */
debug_zval_dump();

echo "Done\n";

?>

--EXPECTF--
*** Testing error conditions ***

Warning: debug_zval_dump() expects at least %d parameter, %d given in %s on line %d
Done
