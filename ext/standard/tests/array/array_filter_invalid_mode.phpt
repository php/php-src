--TEST--
Test array_filter() function : usage variations - mode exception
--FILE--
<?php

try {
    var_dump(array_filter([], mode: 999));
} catch (Throwable $e) {
    echo $e::class . ': '.$e->getMessage(), "\n";
}

echo "Done"
?>
--EXPECT--
ValueError: array_filter(): Argument #3 ($mode) must be a valid mode
Done
