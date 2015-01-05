--TEST--
Test strspn() function : basic functionality 
--FILE--
<?php
/* Prototype  : proto int strspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters found in mask.
                If start or/and length is provided, it works like strspn(substr($s,$start,$len),$good_chars) 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing strspn() : basic functionality
*/

echo "*** Testing strspn() : basic functionality ***\n";


// Initialise all required variables
$str = "this is the test string";
$mask = "htes ";
$start = 8;
$len = 30;

// Calling strspn() with all possible arguments
var_dump( strspn($str, $mask, $start, $len) );

// Calling strspn() with three arguments and default len argument
var_dump( strspn($str, $mask, $start) );

// Calling strspn() with default arguments
var_dump( strspn($str, $mask) );

echo "Done"
?>
--EXPECTF--
*** Testing strspn() : basic functionality ***
int(11)
int(11)
int(2)
Done