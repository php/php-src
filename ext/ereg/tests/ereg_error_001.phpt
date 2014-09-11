--TEST--
Test ereg() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto int ereg(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test wrong number of args
 */

echo "*** Testing ereg() : error conditions ***\n";


//Test ereg with one more than the expected number of arguments
echo "\n-- Testing ereg() function with more than expected no. of arguments --\n";
$pattern = 'string_val';
$string = 'string_val';
$registers = array(1, 2);
$extra_arg = 10;
var_dump( ereg($pattern, $string, $registers, $extra_arg) );

// Testing ereg with one less than the expected number of arguments
echo "\n-- Testing ereg() function with less than expected no. of arguments --\n";
$pattern = 'string_val';
var_dump( ereg($pattern) );

echo "Done";
?>
--EXPECTF--
*** Testing ereg() : error conditions ***

-- Testing ereg() function with more than expected no. of arguments --

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing ereg() function with less than expected no. of arguments --

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
