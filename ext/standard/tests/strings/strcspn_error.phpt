--TEST--
Test strcspn() function : error conditions 
--FILE--
<?php
/* Prototype  : proto int strcspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters not found in mask.
                If start or/and length is provided works like strcspn(substr($s,$start,$len),$bad_chars) 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Test strcspn() : for error conditons
*/

echo "*** Testing strcspn() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing strcspn() function with Zero arguments --\n";
var_dump( strcspn() );

//Test strcspn with one more than the expected number of arguments
echo "\n-- Testing strcspn() function with more than expected no. of arguments --\n";
$str = 'string_val';
$mask = 'string_val';
$start = 2;
$len = 20;


$extra_arg = 10;
var_dump( strcspn($str,$mask,$start,$len, $extra_arg) );

// Testing strcspn withone less than the expected number of arguments
echo "\n-- Testing strcspn() function with less than expected no. of arguments --\n";
$str = 'string_val';
var_dump( strcspn($str) );

echo "Done"
?>
--EXPECTF--
*** Testing strcspn() : error conditions ***

-- Testing strcspn() function with Zero arguments --

Warning: strcspn() expects at least 2 parameters, 0 given in %s on line %d
NULL

-- Testing strcspn() function with more than expected no. of arguments --

Warning: strcspn() expects at most 4 parameters, 5 given in %s on line %d
NULL

-- Testing strcspn() function with less than expected no. of arguments --

Warning: strcspn() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
