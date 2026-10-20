--TEST--
Test stripos() function : error conditions
--FILE--
<?php
echo "*** Testing stripos() function: error conditions ***\n";

echo "\n-- Offset beyond the end of the string --\n";
try {
    stripos("Hello World", "o", 12);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

echo "\n-- Offset before the start of the string --\n";
try {
    stripos("Hello World", "o", -12);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: error conditions ***

-- Offset beyond the end of the string --
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

-- Offset before the start of the string --
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
*** Done ***
