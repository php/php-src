--TEST--
Test strpbrk() function : error conditions 
--FILE--
<?php
/* Prototype  : array strpbrk(string haystack, string char_list)
 * Description: Search a string for any of a set of characters 
 * Source code: ext/standard/string.c
 * Alias to functions: 
 */

echo "*** Testing strpbrk() : error conditions ***\n";

$haystack = 'This is a Simple text.';
$char_list = 'string_val';
$extra_arg = 10;

echo "\n-- Testing strpbrk() function with more than expected no. of arguments --\n";
var_dump( strpbrk($haystack, $char_list, $extra_arg) );

echo "\n-- Testing strpbrk() function with less than expected no. of arguments --\n";
var_dump( strpbrk($haystack) );

echo "\n-- Testing strpbrk() function with empty second argument --\n";
var_dump( strpbrk($haystack, '') );

echo "\n-- Testing strpbrk() function with arrays --\n";
var_dump( strpbrk($haystack, array('a', 'b', 'c') ) );
var_dump( strpbrk(array('foo', 'bar'), 'b') );

?>
===DONE===
--EXPECTF--
*** Testing strpbrk() : error conditions ***

-- Testing strpbrk() function with more than expected no. of arguments --

Warning: strpbrk() expects exactly 2 parameters, 3 given in %s on line %d
bool(false)

-- Testing strpbrk() function with less than expected no. of arguments --

Warning: strpbrk() expects exactly 2 parameters, 1 given in %s on line %d
bool(false)

-- Testing strpbrk() function with empty second argument --

Warning: strpbrk(): The character list cannot be empty in %s on line %d
bool(false)

-- Testing strpbrk() function with arrays --

Warning: strpbrk() expects parameter 2 to be string, array given in %s on line %d
bool(false)

Warning: strpbrk() expects parameter 1 to be string, array given in %s on line %d
bool(false)
===DONE===
