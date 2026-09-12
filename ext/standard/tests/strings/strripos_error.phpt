--TEST--
Test strripos() function : error conditions
--FILE--
<?php
echo "*** Testing strripos() function: error conditions ***\n";

echo "\n-- Offset beyond the end of the string --\n";
try {
    strripos("Hello World", "o", 12);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strripos("Hello World", "o", PHP_INT_MAX);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n-- Offset before the start of the string --\n";
try {
    strripos("Hello World", "o", -12);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strripos("Hello World", "o", PHP_INT_MIN);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "*** Done ***";
?>
--EXPECT--
*** Testing strripos() function: error conditions ***

-- Offset beyond the end of the string --
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

-- Offset before the start of the string --
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
*** Done ***
