--TEST--
Test mb_stristr() function : error conditions 
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stristr') or die("skip mb_stristr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_stristr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds first occurrence of a string within another, case insensitive 
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions: 
 */

echo "*** Testing mb_stristr() : error conditions ***\n";


//Test mb_stristr with one more than the expected number of arguments
echo "\n-- Testing mb_stristr() function with more than expected no. of arguments --\n";
$haystack = b'string_val';
$needle = b'string_val';
$part = true;
$encoding = 'string_val';
$extra_arg = 10;
var_dump( mb_stristr($haystack, $needle, $part, $encoding, $extra_arg) );

// Testing mb_stristr with one less than the expected number of arguments
echo "\n-- Testing mb_stristr() function with less than expected no. of arguments --\n";
$haystack = b'string_val';
var_dump( mb_stristr($haystack) );

?>
===DONE===
--EXPECTF--
*** Testing mb_stristr() : error conditions ***

-- Testing mb_stristr() function with more than expected no. of arguments --

Warning: mb_stristr() expects at most 4 parameters, 5 given in %s on line %d
bool(false)

-- Testing mb_stristr() function with less than expected no. of arguments --

Warning: mb_stristr() expects at least 2 parameters, 1 given in %s on line %d
bool(false)
===DONE===
