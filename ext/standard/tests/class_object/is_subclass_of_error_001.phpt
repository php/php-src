--TEST--
Test is_subclass_of() function : wrong number of args 
--FILE--
<?php
/* Prototype  : proto bool is_subclass_of(object object, string class_name)
 * Description: Returns true if the object has this class as one of its parents 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing is_subclass_of() : error conditions ***\n";


//Test is_subclass_of with one more than the expected number of arguments
echo "\n-- Testing is_subclass_of() function with more than expected no. of arguments --\n";
$object = new stdclass();
$class_name = 'string_val';
$extra_arg = 10;
var_dump( is_subclass_of($object, $class_name, $extra_arg) );

// Testing is_subclass_of with one less than the expected number of arguments
echo "\n-- Testing is_subclass_of() function with less than expected no. of arguments --\n";
$object = new stdclass();
var_dump( is_subclass_of($object) );

echo "Done";
?>
--EXPECTF--
*** Testing is_subclass_of() : error conditions ***

-- Testing is_subclass_of() function with more than expected no. of arguments --

Warning: is_subclass_of() expects exactly 2 parameters, 3 given in %s on line 16
NULL

-- Testing is_subclass_of() function with less than expected no. of arguments --

Warning: is_subclass_of() expects exactly 2 parameters, 1 given in %s on line 21
NULL
Done
