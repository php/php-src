--TEST--
Test substr_count() function (error conditions)
--FILE--
<?php

echo "\n*** Testing error conditions ***\n";
$str = 'abcdefghik';

/* offset before start */
try {
    substr_count($str, "t", -20);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

/* offset > size of the string */
try {
    substr_count($str, "t", 25);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

/* offset very big */
try {
    substr_count($str, 'b', PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    substr_count($str, 'b', PHP_INT_MIN);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

/* Using offset and length to go beyond the size of the string:
   Exception is expected, as length+offset > length of string */
try {
    substr_count($str, "i", 5, 7);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

/* length too small */
try {
    substr_count($str, "t", 2, -20);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

/* length very big */
try {
    substr_count($str, 'b', 0, PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    substr_count($str, 'b', 0, PHP_INT_MIN);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";

?>
--EXPECT--
*** Testing error conditions ***
substr_count(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
substr_count(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
substr_count(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
substr_count(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
substr_count(): Argument #4 ($length) must be contained in argument #1 ($haystack)
substr_count(): Argument #4 ($length) must be contained in argument #1 ($haystack)
substr_count(): Argument #4 ($length) must be contained in argument #1 ($haystack)
substr_count(): Argument #4 ($length) must be contained in argument #1 ($haystack)
Done
