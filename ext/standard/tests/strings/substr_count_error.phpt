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

/* Using offset and length to go beyond the size of the string:
   Warning message expected, as length+offset > length of string */
var_dump( substr_count($str, "i", 5, 7) );

/* length too small */
var_dump( substr_count($str, "t", 2, -20) );

echo "Done\n";

?>
--EXPECTF--
*** Testing error conditions ***
substr_count(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
substr_count(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

Warning: substr_count(): Invalid length value in %s on line %d
bool(false)

Warning: substr_count(): Invalid length value in %s on line %d
bool(false)
Done
