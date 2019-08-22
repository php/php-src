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
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***
Invalid argument, levels must be >= 1
Done
