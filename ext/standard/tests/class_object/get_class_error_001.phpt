--TEST--
Test get_class() function : error conditions - wrong number of arguments.
--FILE--
<?php
/* Prototype  : proto string get_class([object object])
 * Description: Retrieves the class name 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing get_class() : error conditions ***\n";

//Test get_class with one more than the expected number of arguments
echo "\n-- Testing get_class() function with more than expected no. of arguments --\n";
$object = new stdclass();
$extra_arg = 10;
var_dump( get_class($object, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing get_class() : error conditions ***

-- Testing get_class() function with more than expected no. of arguments --

Warning: Wrong parameter count for get_class() in %s on line 14
NULL
Done