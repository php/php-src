--TEST--
Test get_class_vars() function : error conditions
--FILE--
<?php
/* Prototype  : array get_class_vars(string class_name)
 * Description: Returns an array of default properties of the class.
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing get_class_vars() : error conditions ***\n";


//Test get_class_vars with one more than the expected number of arguments
echo "\n-- Testing get_class_vars() function with more than expected no. of arguments --\n";
$obj = new stdclass();
$extra_arg = 10;
var_dump(get_class_vars($obj,$extra_arg) );

// Testing get_class_vars with one less than the expected number of arguments
echo "\n-- Testing get_class_vars() function with less than expected no. of arguments --\n";
var_dump(get_class_vars());

?>
===DONE===
--EXPECTF--
*** Testing get_class_vars() : error conditions ***

-- Testing get_class_vars() function with more than expected no. of arguments --

Warning: get_class_vars() expects exactly 1 parameter, 2 given in %sget_class_vars_error.php on line %d
NULL

-- Testing get_class_vars() function with less than expected no. of arguments --

Warning: get_class_vars() expects exactly 1 parameter, 0 given in %sget_class_vars_error.php on line %d
NULL
===DONE===