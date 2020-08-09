--TEST--
Test fileinode() function: Error conditions
--FILE--
<?php
echo "*** Testing error conditions of fileinode() ***";

/* Non-existing file or dir */
var_dump( fileinode("/no/such/file/dir") );

/* Invalid arguments */
var_dump( fileinode("string") );
var_dump( fileinode(100) );

echo "\n*** Done ***";
?>
--EXPECTF--
*** Testing error conditions of fileinode() ***
Warning: fileinode(): stat failed for /no/such/file/dir in %s on line %d
bool(false)

Warning: fileinode(): stat failed for string in %s on line %d
bool(false)

Warning: fileinode(): stat failed for 100 in %s on line %d
bool(false)

*** Done ***
