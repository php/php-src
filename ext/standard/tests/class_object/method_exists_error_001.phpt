--TEST--
Test method_exists() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto bool method_exists(object object, string method)
 * Description: Checks if the class method exists
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing method_exists() : error conditions ***\n";


//Test method_exists with one more than the expected number of arguments
echo "\n-- Testing method_exists() function with more than expected no. of arguments --\n";
$object = new stdclass();
$method = 'string_val';
$extra_arg = 10;
var_dump( method_exists($object, $method, $extra_arg) );

// Testing method_exists with one less than the expected number of arguments
echo "\n-- Testing method_exists() function with less than expected no. of arguments --\n";
$object = new stdclass();
var_dump( method_exists($object) );

echo "Done";
?>
--EXPECTF--
*** Testing method_exists() : error conditions ***

-- Testing method_exists() function with more than expected no. of arguments --

Warning: method_exists() expects exactly 2 parameters, 3 given in %s on line 16
NULL

-- Testing method_exists() function with less than expected no. of arguments --

Warning: method_exists() expects exactly 2 parameters, 1 given in %s on line 21
NULL
Done
