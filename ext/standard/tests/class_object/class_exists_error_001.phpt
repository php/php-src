--TEST--
Test class_exists() function : error conditions (wrong number of arguments)
--FILE--
<?php
/* Prototype  : proto bool class_exists(string classname [, bool autoload])
 * Description: Checks if the class exists 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

/**
 * Test wrong number of arguments
 */

echo "*** Testing class_exists() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing class_exists() function with Zero arguments --\n";
var_dump( class_exists() );

//Test class_exists with one more than the expected number of arguments
echo "\n-- Testing class_exists() function with more than expected no. of arguments --\n";
$classname = 'string_val';
$autoload = true;
$extra_arg = 10;
var_dump( class_exists($classname, $autoload, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing class_exists() : error conditions ***

-- Testing class_exists() function with Zero arguments --

Warning: class_exists() expects at least 1 parameter, 0 given in %s on line 16
NULL

-- Testing class_exists() function with more than expected no. of arguments --

Warning: class_exists() expects at most 2 parameters, 3 given in %s on line 23
NULL
Done
