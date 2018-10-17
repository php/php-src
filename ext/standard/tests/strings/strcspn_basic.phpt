--TEST--
Test strcspn() function : basic functionality
--FILE--
<?php
/* Prototype  : proto int strcspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters not found in mask.
                If start or/and length is provided, it works like strcspn(substr($s,$start,$len),$bad_chars)
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing strcspn() : basic functionality
*/

echo "*** Testing strcspn() : basic functionality ***\n";


// Initialise all required variables
$str = "this is the test string";
$mask = "es";
$start = 15;
$len = 30;

// Calling strcspn() with all possible arguments
var_dump( strcspn($str, $mask, $start, $len) );

// Calling strcspn() with three arguments
var_dump( strcspn($str, $mask, $start) );

// Calling strcspn() with default arguments
var_dump( strcspn($str, $mask) );

echo "Done"
?>
--EXPECT--
*** Testing strcspn() : basic functionality ***
int(2)
int(2)
int(3)
Done
