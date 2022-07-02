--TEST--
Test filetype() function: Error conditions
--FILE--
<?php
echo "*** Testing error conditions ***";
/* non-existing file or dir */
print( filetype("/no/such/file/dir") );

/* unknown type */
print( filetype("string") );
print( filetype(100) );

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing error conditions ***
Warning: filetype(): Lstat failed for /no/such/file/dir in %s on line %d

Warning: filetype(): Lstat failed for string in %s on line %d

Warning: filetype(): Lstat failed for 100 in %s on line %d

*** Done ***
