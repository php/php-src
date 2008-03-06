--TEST--
Test get_parent_class() function : error conditions - wrong number of args.
--FILE--
<?php
/* Prototype  : proto string get_parent_class([mixed object])
 * Description: Retrieves the parent class name for object or class or current scope. 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing get_parent_class() : error conditions ***\n";


//Test get_parent_class with one more than the expected number of arguments
echo "\n-- Testing get_parent_class() function with more than expected no. of arguments --\n";
$object = 1;
$extra_arg = 10;
var_dump( get_parent_class($object, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing get_parent_class() : error conditions ***

-- Testing get_parent_class() function with more than expected no. of arguments --

Warning: Wrong parameter count for get_parent_class() in %s on line 15
NULL
Done