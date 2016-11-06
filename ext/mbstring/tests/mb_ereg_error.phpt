--TEST--
Test mb_ereg() function : error conditions - pass incorrect number of arguments
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_ereg(string $pattern, string $string [, array $registers])
 * Description: Regular expression match for multibyte string 
 * Source code: ext/mbstring/php_mbregex.c
 */

/*
 * Test behaviour of mb_ereg() when passed an incorrcect number of arguments
 */

echo "*** Testing mb_ereg() : error conditions ***\n";


//Test mb_ereg with one more than the expected number of arguments
echo "\n-- Testing mb_ereg() function with more than expected no. of arguments --\n";
$pattern = 'string_val';
$string = 'string_val';
$registers = array(1, 2);
$extra_arg = 10;
var_dump( mb_ereg($pattern, $string, $registers, $extra_arg) );

// Testing mb_ereg with one less than the expected number of arguments
echo "\n-- Testing mb_ereg() function with less than expected no. of arguments --\n";
$pattern = 'string_val';
var_dump( mb_ereg($pattern) );

echo "Done";
?>
--EXPECTF--
*** Testing mb_ereg() : error conditions ***

-- Testing mb_ereg() function with more than expected no. of arguments --

Warning: mb_ereg() expects at most 3 parameters, 4 given in %s on line %d
bool(false)

-- Testing mb_ereg() function with less than expected no. of arguments --

Warning: mb_ereg() expects at least 2 parameters, 1 given in %s on line %d
bool(false)
Done
