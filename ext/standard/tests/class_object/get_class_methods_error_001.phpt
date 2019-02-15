--TEST--
Test get_class_methods() function : error conditions
--FILE--
<?php
/* Prototype  : proto array get_class_methods(mixed class)
 * Description: Returns an array of method names for class or class instance.
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

/*
 * Test wrong number of arguments.
 */

echo "*** Testing get_class_methods() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing get_class_methods() function with Zero arguments --\n";
var_dump( get_class_methods() );

//Test get_class_methods with one more than the expected number of arguments
echo "\n-- Testing get_class_methods() function with more than expected no. of arguments --\n";
$class = 1;
$extra_arg = 10;
var_dump( get_class_methods($class, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing get_class_methods() : error conditions ***

-- Testing get_class_methods() function with Zero arguments --

Warning: get_class_methods() expects exactly 1 parameter, 0 given in %s on line 16
NULL

-- Testing get_class_methods() function with more than expected no. of arguments --

Warning: get_class_methods() expects exactly 1 parameter, 2 given in %s on line 22
NULL
Done
