--TEST--
Test filegroup() function: error conditions
--FILE--
<?php
/* Prototype: int filegroup ( string $filename )
 *  Description: Returns the group ID of the file, or FALSE in case of an error.
 */

echo "*** Testing filegroup(): error conditions ***\n";

/* Non-existing file or dir */
var_dump( filegroup("/no/such/file/dir") );

/* Invalid arguments */
var_dump( filegroup("string") );
var_dump( filegroup(100) );

/* Invalid no.of arguments */
var_dump( filegroup() );  // args < expected
var_dump( filegroup("/no/such/file", "root") );  // args > expected

echo "\n*** Done ***\n";
?>

--EXPECTF--
*** Testing filegroup(): error conditions ***

Warning: filegroup(): stat failed for /no/such/file/dir in %s on line %d
bool(false)

Warning: filegroup(): stat failed for string in %s on line %d
bool(false)

Warning: filegroup(): stat failed for 100 in %s on line %d
bool(false)

Warning: Wrong parameter count for filegroup() in %s on line %d
NULL

Warning: Wrong parameter count for filegroup() in %s on line %d
NULL

*** Done ***
