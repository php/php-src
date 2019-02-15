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

// Bad arg
var_dump( dirname("/var/tmp/bar.gz", 0) );

// more than expected no. of arguments
var_dump( dirname("/var/tmp/bar.gz", 1, ".gz") );

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***

Warning: dirname() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: dirname(): Invalid argument, levels must be >= 1 in %s on line %d
NULL

Warning: dirname() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
