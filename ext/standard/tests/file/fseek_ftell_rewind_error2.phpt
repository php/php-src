--TEST--
Test fseek(), ftell() & rewind() functions : error conditions - ftell()
--FILE--
<?php

echo "*** Testing ftell() : error conditions ***\n";

// ftell on a file handle which is already closed
echo "-- Testing ftell with closed/unset file handle --\n";
$fp = fopen(__FILE__, "r");
fclose($fp);
try {
    var_dump(ftell($fp));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
*** Testing ftell() : error conditions ***
-- Testing ftell with closed/unset file handle --
TypeError: ftell(): Argument #1 ($stream) must be an open stream resource
Done
