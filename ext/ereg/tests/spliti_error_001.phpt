--TEST--
Test spliti() function : error conditions - wrong number of args 
--FILE--
<?php
/* Prototype  : proto array spliti(string pattern, string string [, int limit])
 * Description: spliti string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

echo "*** Testing spliti() : error conditions - wrong number of args ***\n";


//Test spliti with one more than the expected number of arguments
echo "\n-- Testing spliti() function with more than expected no. of arguments --\n";
$pattern = 'string_val';
$string = 'string_val';
$limit = 10;
$extra_arg = 10;
var_dump( spliti($pattern, $string, $limit, $extra_arg) );

// Testing spliti with one less than the expected number of arguments
echo "\n-- Testing spliti() function with less than expected no. of arguments --\n";
$pattern = 'string_val';
var_dump( spliti($pattern) );

echo "Done";
?>
--EXPECTF--
*** Testing spliti() : error conditions - wrong number of args ***

-- Testing spliti() function with more than expected no. of arguments --

Deprecated: Function spliti() is deprecated in %s on line %d

Warning: spliti() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing spliti() function with less than expected no. of arguments --

Deprecated: Function spliti() is deprecated in %s on line %d

Warning: spliti() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
