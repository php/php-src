--TEST--
Test base_convert() function :  error conditions - incorrect input
--FILE--
<?php
echo "*** Testing base_convert() : error conditions ***\n";

// get a class
class classA
{
}

try {
    base_convert(1234, 1, 10);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    base_convert(1234, 10, 37);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    base_convert(new classA(), 8, 10);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing base_convert() : error conditions ***
base_convert(): Argument #2 ($from_base) must be between 2 and 36 (inclusive)
base_convert(): Argument #3 ($to_base) must be between 2 and 36 (inclusive)
base_convert(): Argument #1 ($num) must be of type string, classA given
