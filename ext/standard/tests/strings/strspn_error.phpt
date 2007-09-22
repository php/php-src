--TEST--
Test strspn() function : error conditions 
--FILE--
<?php
/* Prototype  : proto int strspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters found in mask.
                If start or/and length is provided works like strspn(substr($s,$start,$len),$good_chars) 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Test strspn() : for error conditons
*/

echo "*** Testing strspn() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing strspn() function with Zero arguments --\n";
var_dump( strspn() );

//Test strspn with one more than the expected number of arguments
echo "\n-- Testing strspn() function with more than expected no. of arguments --\n";
$str = 'string_val';
$mask = 'string_val';
$start = 2;
$len = 20;


$extra_arg = 10;
var_dump( strspn($str,$mask,$start,$len, $extra_arg) );

// Testing strspn withone less than the expected number of arguments
echo "\n-- Testing strspn() function with less than expected no. of arguments --\n";
$str = 'string_val';
var_dump( strspn($str) );

echo "Done"
?>
--EXPECTF--
*** Testing strspn() : error conditions ***

-- Testing strspn() function with Zero arguments --

Warning: strspn() expects at least 2 parameters, 0 given in %s on line %d
NULL

-- Testing strspn() function with more than expected no. of arguments --

Warning: strspn() expects at most 4 parameters, 5 given in %s on line %d
NULL

-- Testing strspn() function with less than expected no. of arguments --

Warning: strspn() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
