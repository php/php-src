--TEST--
Test fpassthru() function: Error conditions
--FILE--
<?php
/* 
Prototype: int fpassthru ( resource $handle );
Description: Reads to EOF on the given file pointer from the current position
  and writes the results to the output buffer.
*/

echo "*** Test error conditions of fpassthru() function ***\n";

/* Non-existing file resource */
$no_file = fread("/no/such/file", "r");
var_dump( fpassthru($no_file) );

/* No.of args less than expected */
var_dump( fpassthru() );

/* No.of args greaer than expected */
var_dump( fpassthru("", "") );

echo "\n*** Done ***\n";

?>
--EXPECTF--
*** Test error conditions of fpassthru() function ***

Warning: fread(): supplied argument is not a valid stream resource in %s on line %d

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: Wrong parameter count for fpassthru() in %s on line %d
NULL

Warning: Wrong parameter count for fpassthru() in %s on line %d
NULL

*** Done ***
