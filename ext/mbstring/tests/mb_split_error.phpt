--TEST--
Test mb_split() function : error conditions 
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_split') or die("skip mb_split() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : proto array mb_split(string pattern, string string [, int limit])
 * Description: split multibyte string into array by regular expression 
 * Source code: ext/mbstring/php_mbregex.c
 * Alias to functions: 
 */

/*
 * test too few and too many parameters
 */

echo "*** Testing mb_split() : error conditions ***\n";


//Test mb_split with one more than the expected number of arguments
echo "\n-- Testing mb_split() function with more than expected no. of arguments --\n";
$pattern = ' ';
$string = 'a b c d e f g';
$limit = 0;
$extra_arg = 10;
var_dump( mb_split($pattern, $string, $limit, $extra_arg) );

// Testing mb_split with one less than the expected number of arguments
echo "\n-- Testing mb_split() function with less than expected no. of arguments --\n";
$pattern = 'string_val';
var_dump( mb_split($pattern) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_split() : error conditions ***

-- Testing mb_split() function with more than expected no. of arguments --

Warning: mb_split() expects at most 3 parameters, 4 given in %s on line %d
bool(false)

-- Testing mb_split() function with less than expected no. of arguments --

Warning: mb_split() expects at least 2 parameters, 1 given in %s on line %d
bool(false)
Done
