--TEST--
Test dirname() function : error conditions
--FILE--
<?php
echo "*** Testing error conditions ***\n";

// Bad arg
try {
    dirname("/var/tmp/bar.gz", 0);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing error conditions ***
ValueError: dirname(): Argument #2 ($levels) must be greater than or equal to 1
