--TEST--
Test dirname() function : error conditions
--FILE--
<?php
/* Prototype: string dirname ( string $path );
   Description: Returns directory name component of path.
*/
echo "*** Testing error conditions ***\n";
// zero arguments 
var_dump( dirname() );

// more than expected no. of arguments
var_dump( dirname("/var/tmp/bar.gz", ".gz") );

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***

Warning: dirname() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: dirname() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
