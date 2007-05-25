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

Warning: Wrong parameter count for dirname() in %s on line %d
NULL

Warning: Wrong parameter count for dirname() in %s on line %d
NULL
Done
