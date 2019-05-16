--TEST--
Test dirname() function : error conditions
--FILE--
<?php
/* Prototype: string dirname ( string $path );
   Description: Returns directory name component of path.
*/
echo "*** Testing error conditions ***\n";

// Bad arg
var_dump( dirname("/var/tmp/bar.gz", 0) );

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***

Warning: dirname(): Invalid argument, levels must be >= 1 in %s on line %d
NULL
Done
