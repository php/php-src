--TEST--
Test trait_exists() function : error conditions (wrong number of arguments)
--FILE--
<?php
/* Prototype  : proto bool trait_exists(string traitname [, bool autoload])
 * Description: Checks if the trait exists
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

/**
 * Test wrong number of arguments
 */

echo "*** Testing trait_exists() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing trait_exists() function with Zero arguments --\n";
var_dump( trait_exists() );

//Test trait_exists with one more than the expected number of arguments
echo "\n-- Testing trait_exists() function with more than expected no. of arguments --\n";
$traitname = 'string_val';
$autoload = true;
$extra_arg = 10;
var_dump( trait_exists($traitname, $autoload, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing trait_exists() : error conditions ***

-- Testing trait_exists() function with Zero arguments --

Warning: trait_exists() expects at least 1 parameter, 0 given in %s on line 16
NULL

-- Testing trait_exists() function with more than expected no. of arguments --

Warning: trait_exists() expects at most 2 parameters, 3 given in %s on line 23
NULL
Done
