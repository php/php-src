--TEST--
Test is_a() function : error conditions - wrong number of args
--FILE--
<?php
/* Prototype  : proto bool is_a(object object, string class_name)
 * Description: Returns true if the object is of this class or has this class as one of its parents 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing is_a() : error conditions ***\n";


//Test is_a with one more than the expected number of arguments
echo "\n-- Testing is_a() function with more than expected no. of arguments --\n";
$object = new stdclass();
$class_name = 'string_val';
$extra_arg = 10;
var_dump( is_a($object, $class_name, $extra_arg) );

// Testing is_a with one less than the expected number of arguments
echo "\n-- Testing is_a() function with less than expected no. of arguments --\n";
$object = new stdclass();
var_dump( is_a($object) );

echo "Done";
?>
--EXPECTF--
*** Testing is_a() : error conditions ***

-- Testing is_a() function with more than expected no. of arguments --

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 16

Warning: Wrong parameter count for is_a() in %s on line 16
NULL

-- Testing is_a() function with less than expected no. of arguments --

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %s on line 21

Warning: Wrong parameter count for is_a() in %s on line 21
NULL
Done