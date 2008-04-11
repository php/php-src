--TEST--
Test eregi() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto int eregi(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test wrong number of args
 */

echo "*** Testing eregi() : error conditions ***\n";


//Test eregi with one more than the expected number of arguments
echo "\n-- Testing eregi() function with more than expected no. of arguments --\n";
$pattern = 'string_val';
$string = 'string_val';
$registers = array(1, 2);
$extra_arg = 10;
var_dump( eregi($pattern, $string, $registers, $extra_arg) );

// Testing eregi with one less than the expected number of arguments
echo "\n-- Testing eregi() function with less than expected no. of arguments --\n";
$pattern = 'string_val';
var_dump( eregi($pattern) );

echo "Done";
?>
--EXPECTF--
*** Testing eregi() : error conditions ***

-- Testing eregi() function with more than expected no. of arguments --

Warning: Wrong parameter count for eregi() in %s on line 21
NULL

-- Testing eregi() function with less than expected no. of arguments --

Warning: Wrong parameter count for eregi() in %s on line 26
NULL
Done
