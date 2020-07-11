--TEST--
Test filegroup() function: error conditions
--FILE--
<?php

echo "*** Testing filegroup(): error conditions ***\n";

/* Non-existing file or dir */
var_dump( filegroup("/no/such/file/dir") );

/* Invalid arguments */
var_dump( filegroup("string") );
var_dump( filegroup(100) );

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

*** Done ***
