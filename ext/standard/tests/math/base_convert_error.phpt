--TEST--
Test base_convert() function :  error conditions - incorrect input
--FILE--
<?php
/* Prototype  : string base_convert  ( string $number  , int $frombase  , int $tobase  )
 * Description: Convert a number between arbitrary bases.
 * Source code: ext/standard/math.c
 */

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
Invalid `from base' (1)
Invalid `to base' (37)
Object of class classA could not be converted to string
