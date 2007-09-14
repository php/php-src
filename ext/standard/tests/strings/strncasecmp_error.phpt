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
$len = 10;
$extra_arg = 10;

echo "\n-- Testing strncasecmp() function with Zero arguments --";
var_dump( strncasecmp() );

echo "\n-- Testing strncasecmp() function with less than expected number of arguments --";
var_dump( strncasecmp($str1) );
var_dump( strncasecmp($str1, $str2) );

echo "\n-- Testing strncasecmp() function with more than expected number of arguments --";
var_dump( strncasecmp($str1, $str2, $len, $extra_arg) );

echo "\n-- Testing strncasecmp() function with invalid argument --";
$len = -10;
var_dump( strncasecmp($str1, $str2, $len) );
echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing strncasecmp() function: error conditions ***

-- Testing strncasecmp() function with Zero arguments --
Warning: Wrong parameter count for strncasecmp() in %s on line %d
NULL

-- Testing strncasecmp() function with less than expected number of arguments --
Warning: Wrong parameter count for strncasecmp() in %s on line %d
NULL

Warning: Wrong parameter count for strncasecmp() in %s on line %d
NULL

-- Testing strncasecmp() function with more than expected number of arguments --
Warning: Wrong parameter count for strncasecmp() in %s on line %d
NULL

-- Testing strncasecmp() function with invalid argument --
Warning: Length must be greater than or equal to 0 in %s on line %d
bool(false)
*** Done ***
