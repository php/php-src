--TEST--
Test of fileowner() function: error conditions
--FILE--
<?php
/* Prototype: int fileowner ( string $filename )
 * Description: Returns the user ID of the owner of the file, or
 *              FALSE in case of an error.
 */

echo "*** Testing fileowner(): error conditions ***\n";
/* Non-existing file or dir */
var_dump( fileowner("/no/such/file/dir") );

/* Invalid arguments */
var_dump( fileowner("string") );
var_dump( fileowner(100) );

/* Invalid no.of arguments */
var_dump( fileowner() );  // args < expected
var_dump( fileowner("/no/such/file", "root") );  // args > expected

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing fileowner(): error conditions ***

Warning: fileowner(): stat failed for /no/such/file/dir in %s on line %d
bool(false)

Warning: fileowner(): stat failed for string in %s on line %d
bool(false)

Warning: fileowner(): stat failed for 100 in %s on line %d
bool(false)

Warning: fileowner() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: fileowner() expects exactly 1 parameter, 2 given in %s on line %d
NULL

*** Done ***
