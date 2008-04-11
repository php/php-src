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

Warning: Wrong parameter count for spliti() in %s on line 17
NULL

-- Testing spliti() function with less than expected no. of arguments --

Warning: Wrong parameter count for spliti() in %s on line 22
NULL
Done