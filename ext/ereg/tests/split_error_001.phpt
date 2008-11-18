--TEST--
Test split() function : error conditions - wrong number of args 
--FILE--
<?php
/* Prototype  : proto array split(string pattern, string string [, int limit])
 * Description: Split string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

echo "*** Testing split() : error conditions - wrong number of args ***\n";


//Test split with one more than the expected number of arguments
echo "\n-- Testing split() function with more than expected no. of arguments --\n";
$pattern = 'string_val';
$string = 'string_val';
$limit = 10;
$extra_arg = 10;
var_dump( split($pattern, $string, $limit, $extra_arg) );

// Testing split with one less than the expected number of arguments
echo "\n-- Testing split() function with less than expected no. of arguments --\n";
$pattern = 'string_val';
var_dump( split($pattern) );

echo "Done";
?>
--EXPECTF--
*** Testing split() : error conditions - wrong number of args ***

-- Testing split() function with more than expected no. of arguments --

Deprecated: Function split() is deprecated in %s on line %d

Warning: split() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing split() function with less than expected no. of arguments --

Deprecated: Function split() is deprecated in %s on line %d

Warning: split() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
