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

*** Done ***
