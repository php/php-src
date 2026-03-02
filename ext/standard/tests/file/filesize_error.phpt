--TEST--
Test filesize() function: error conditions
--FILE--
<?php
echo "*** Testing filesize(): error conditions ***";

/* Non-existing file or dir */
var_dump( filesize("/no/such/file") );
var_dump( filesize("/no/such/dir") );

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing filesize(): error conditions ***
Warning: filesize(): stat failed for /no/such/file in %s on line %d
bool(false)

Warning: filesize(): stat failed for /no/such/dir in %s on line %d
bool(false)
*** Done ***
