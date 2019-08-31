--TEST--
Test fileatime(), filemtime(), filectime() & touch() functions : error conditions
--FILE--
<?php
/*
   Prototype: int fileatime ( string $filename );
   Description: Returns the time the file was last accessed, or FALSE
     in case of an error. The time is returned as a Unix timestamp.

   Prototype: int filemtime ( string $filename );
   Description: Returns the time the file was last modified, or FALSE
     in case of an error.

   Prototype: int filectime ( string $filename );
   Description: Returns the time the file was last changed, or FALSE
     in case of an error. The time is returned as a Unix timestamp.

   Prototype: bool touch ( string $filename [, int $time [, int $atime]] );
   Description: Attempts to set the access and modification times of the file
     named in the filename parameter to the value given in time.
*/

echo "*** Testing error conditions ***\n";

echo "\n-- Testing with  Non-existing files --";
/* Both invalid arguments */
var_dump( fileatime("/no/such/file/or/dir") );
var_dump( filemtime("/no/such/file/or/dir") );
var_dump( filectime("/no/such/file/or/dir") );
var_dump( touch("/no/such/file/or/dir", 10) );

echo "\nDone";
?>
--EXPECTF--
*** Testing error conditions ***

-- Testing with  Non-existing files --
Warning: fileatime(): stat failed for /no/such/file/or/dir in %s on line %d
bool(false)

Warning: filemtime(): stat failed for /no/such/file/or/dir in %s on line %d
bool(false)

Warning: filectime(): stat failed for /no/such/file/or/dir in %s on line %d
bool(false)

Warning: touch(): Unable to create file /no/such/file/or/dir because No such file or directory in %s on line %d
bool(false)

Done
