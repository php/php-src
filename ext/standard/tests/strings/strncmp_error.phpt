--TEST--
Test strncmp() function : error conditions
--FILE--
<?php
/* Prototype  : int strncmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-sensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncmp() function with more/less number of args and invalid args */

echo "*** Testing strncmp() function: error conditions ***\n";
$str1 = 'string_val';
$str2 = 'string_val';

/* Invalid argument for $len */
$len = -10;

try {
    var_dump( strncmp($str1, $str2, $len) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing strncmp() function: error conditions ***
strncmp(): Argument #3 ($len) must be greater than or equal to 0
