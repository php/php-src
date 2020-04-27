--TEST--
Test sscanf() function : error conditions
--FILE--
<?php

/* Prototype  : mixed sscanf  ( string $str  , string $format  [, mixed &$...  ] )
 * Description: Parses input from a string according to a format
 * Source code: ext/standard/string.c
*/
echo "*** Testing sscanf() : error conditions ***\n";

$str = "Hello World";
$format = "%s %s";

echo "\n-- Testing sscanf() function with more than expected no. of arguments --\n";

try {
    sscanf($str, $format, $str1, $str2, $extra_str);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
*** Testing sscanf() : error conditions ***

-- Testing sscanf() function with more than expected no. of arguments --
Variable is not assigned by any conversion specifiers
