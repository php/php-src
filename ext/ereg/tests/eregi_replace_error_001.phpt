--TEST--
Test eregi_replace() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto string eregi_replace(string pattern, string replacement, string string)
 * Description: Replace regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

echo "*** Testing eregi_replace() : error conditions ***\n";


//Test eregi_replace with one more than the expected number of arguments
echo "\n-- Testing eregi_replace() function with more than expected no. of arguments --\n";
$pattern = 'string_val';
$replacement = 'string_val';
$string = 'string_val';
$extra_arg = 10;
var_dump( eregi_replace($pattern, $replacement, $string, $extra_arg) );

// Testing eregi_replace with one less than the expected number of arguments
echo "\n-- Testing eregi_replace() function with less than expected no. of arguments --\n";
$pattern = 'string_val';
$replacement = 'string_val';
var_dump( eregi_replace($pattern, $replacement) );

echo "Done";
?>
--EXPECTF--
*** Testing eregi_replace() : error conditions ***

-- Testing eregi_replace() function with more than expected no. of arguments --

Deprecated: Function eregi_replace() is deprecated in %s on line %d

Warning: eregi_replace() expects exactly 3 parameters, 4 given in %s on line %d
NULL

-- Testing eregi_replace() function with less than expected no. of arguments --

Deprecated: Function eregi_replace() is deprecated in %s on line %d

Warning: eregi_replace() expects exactly 3 parameters, 2 given in %s on line %d
NULL
Done
