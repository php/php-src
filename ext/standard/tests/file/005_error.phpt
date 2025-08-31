--TEST--
Test fileatime(), filemtime(), filectime() & touch() functions : error conditions
--FILE--
<?php

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
