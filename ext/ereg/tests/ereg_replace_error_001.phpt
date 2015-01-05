--TEST--
Test ereg_replace() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto string ereg_replace(string pattern, string replacement, string string)
 * Description: Replace regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

echo "*** Testing ereg_replace() : error conditions ***\n";


//Test ereg_replace with one more than the expected number of arguments
echo "\n-- Testing ereg_replace() function with more than expected no. of arguments --\n";
$pattern = 'string_val';
$replacement = 'string_val';
$string = 'string_val';
$extra_arg = 10;
var_dump( ereg_replace($pattern, $replacement, $string, $extra_arg) );

// Testing ereg_replace with one less than the expected number of arguments
echo "\n-- Testing ereg_replace() function with less than expected no. of arguments --\n";
$pattern = 'string_val';
$replacement = 'string_val';
var_dump( ereg_replace($pattern, $replacement) );

echo "Done";
?>
--EXPECTF--
*** Testing ereg_replace() : error conditions ***

-- Testing ereg_replace() function with more than expected no. of arguments --

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace() expects exactly 3 parameters, 4 given in %s on line %d
NULL

-- Testing ereg_replace() function with less than expected no. of arguments --

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace() expects exactly 3 parameters, 2 given in %s on line %d
NULL
Done
