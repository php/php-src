--TEST--
Test fileinode() function: Error conditions
--FILE--
<?php
/* 
Prototype: int fileinode ( string $filename );
Description: Returns the inode number of the file, or FALSE in case of an error.
*/

echo "*** Testing error conditions of fileinode() ***";

/* Non-existing file or dir */
var_dump( fileinode("/no/such/file/dir") );

/* Invalid arguments */
var_dump( fileinode("string") );
var_dump( fileinode(100) );

/* No.of argumetns less than expected */
var_dump( fileinode() );

/* No.of argumetns greater than expected */
var_dump( fileinode(__FILE__, "string") );

echo "\n*** Done ***";

--EXPECTF--
*** Testing error conditions of fileinode() ***
Warning: fileinode(): stat failed for /no/such/file/dir in %s on line %d
bool(false)

Warning: fileinode(): stat failed for string in %s on line %d
bool(false)

Warning: Wrong parameter count for fileinode() in %s on line %d
NULL

Warning: Wrong parameter count for fileinode() in %s on line %d
NULL

*** Done ***
