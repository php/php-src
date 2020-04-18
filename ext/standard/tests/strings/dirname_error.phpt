--TEST--
Test dirname() function : error conditions
--FILE--
<?php
/* Prototype: string dirname ( string $path );
   Description: Returns directory name component of path.
*/
echo "*** Testing error conditions ***\n";

// Bad arg
try {
    dirname("/var/tmp/bar.gz", 0);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing error conditions ***
dirname(): Argument #2 ($levels) must be greater than or equal to 1
