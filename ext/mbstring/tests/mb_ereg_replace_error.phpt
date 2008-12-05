--TEST--
Test mb_ereg_replace() function : error conditions 
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : proto string mb_ereg_replace(string pattern, string replacement, string string [, string option])
 * Description: Replace regular expression for multibyte string 
 * Source code: ext/mbstring/php_mbregex.c
 * Alias to functions: 
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing mb_ereg_replace() : error conditions ***\n";


//Test mb_ereg_replace with one more than the expected number of arguments
echo "\n-- Testing mb_ereg_replace() function with more than expected no. of arguments --\n";
$pattern = b'[a-k]';
$replacement = b'1';
$string = b'string_val';
$option = '';
$extra_arg = 10;
var_dump( mb_ereg_replace($pattern, $replacement, $string, $option, $extra_arg) );

// Testing mb_ereg_replace with one less than the expected number of arguments
echo "\n-- Testing mb_ereg_replace() function with less than expected no. of arguments --\n";
$pattern = b'string_val';
$replacement = b'string_val';
var_dump( mb_ereg_replace($pattern, $replacement) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_ereg_replace() : error conditions ***

-- Testing mb_ereg_replace() function with more than expected no. of arguments --

Warning: mb_ereg_replace() expects at most 4 parameters, 5 given in %s on line %d
bool(false)

-- Testing mb_ereg_replace() function with less than expected no. of arguments --

Warning: mb_ereg_replace() expects at least 3 parameters, 2 given in %s on line %d
bool(false)
Done
