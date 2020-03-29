--TEST--
Test strncasecmp() function : error conditions
--FILE--
<?php
/* Prototype  : int strncasecmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-insensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

echo "*** Testing strncasecmp() function: error conditions ***\n";
$str1 = 'string_val';
$str2 = 'string_val';

echo "-- Testing strncasecmp() function with invalid argument --\n";
$len = -10;

try {
    var_dump( strncasecmp($str1, $str2, $len) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing strncasecmp() function: error conditions ***
-- Testing strncasecmp() function with invalid argument --
strncasecmp(): Argument #3 ($len) must be greater than or equal to 0
